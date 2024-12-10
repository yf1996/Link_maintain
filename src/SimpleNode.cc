//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "SimpleNode.h"

#include "inet/mobility/base/MobilityBase.h"

#include <algorithm>
#include <numeric>

using namespace omnetpp;

Define_Module(SimpleNode);

simsignal_t SimpleNode::linkEndSignal = registerSignal("linkEnd");
simsignal_t SimpleNode::sendCountSignal = registerSignal("sendCount");

void SimpleNode::initialize(int stage)
{
    // TODO - Generated method body
    EV_INFO << "SimpleNode::initialize(int stage)" << endl;
    if (stage == INITSTAGE_LOCAL)
    {
        /* code */
        nodeId = this->getIndex();
        EV_INFO << "This node " << nodeId << endl;

        numSectors = par("numSectors");
        slotDuration = par("slotDuration");
        beaconDuration = par("beaconDuration");
        commRange = par("commRange");
        listenningPower = par("listenningPower");
        receivingPower = par("receivingPower");
        transmittingPower = par("transmittingPower");
        basePower = par("basePower");
        batteryStorage = par("batteryStorage");
        transInterval = par("sendInterval");
        nominalVoltage = par("nominalVoltage");
        isPred = par("isPred");

        /**
         * Energy(J) = Battery Capacity(mAh) / 1000 * Voltage(V) * 3600
         */
        batteryStorage_joule = batteryStorage / 1000 * nominalVoltage * 3600;
        EV_INFO << "batteryStorage_joule = " << batteryStorage_joule << endl;

        txTimer = new cMessage("txTimer");
        isDirectionalReceive = false;

        powerMonitorTimer = new cMessage("powerMonitorTimer");

        linkStateThreshold = par("linkStateThreshold");

        beamwidth = 2 * M_PI / numSectors;
        commRangeIncr = 500;
        rangeMultipleMax = par("rangeMultipleMax");

        WATCH(pos);
    }
    else if (stage == INITSTAGE_MOBILITY)
    {
        radioMedium = check_and_cast<RadioMedium *>(findModuleByPath(par("radioMedium")));
        radioMedium->registerNode(this);
    }
    else if (stage == INITSTAGE_ENVIRONMENT)
    {
        /* code */
    }
    else if (stage == INITSTAGE_DOWNLOAD)
    {
    }
    else if (stage == 29)
    {
        // get nbr information
        neighborTable = radioMedium->getNeighborTable(this);
        if (!(*neighborTable).empty())
        {
            EV_INFO << "---------Node " << nodeId << " Neighbor Table--------" << endl;
            EV_INFO << (*neighborTable);

            /* Init txTimer by prediction */
            for (NbrTable::iterator iter = (*neighborTable).begin();
                 iter != (*neighborTable).end();
                 iter++)
            {

                double breakTime;
                auto dst = check_and_cast<SimpleNode *>(iter->first);

                breakTime = getBreakTime(dst);

                // EV_INFO << "dst x pos: " << dst->mobility->getCurrentPosition().getX() << endl;
                // EV_INFO << "dst y pos: " << dst->mobility->getCurrentPosition().getY() << endl;

                // /* test */
                // int rangeMultiple = 0;
                // while (rangeMultiple < rangeMultipleMax)
                // {
                //     double newCommRange = (*neighborTable)[dst].length() + rangeMultiple * commRangeIncr;
                //     double newBeamwidth = getBeamwidthFromCommRange(newCommRange);

                //     double distanceBreakTime = getDistanceBreakTime(dst->mobility->getSpeedXMean(),
                //                                                     dst->mobility->getSpeedYMean(),
                //                                                     dst->mobility->getSpeedXStdDev(),
                //                                                     dst->mobility->getSpeedYStdDev(),
                //                                                     dst->mobility->getCurrentPosition().getX(),
                //                                                     dst->mobility->getCurrentPosition().getY(),
                //                                                     newCommRange,
                //                                                     dst->mobility->getUpdateInterval());

                //     EV_INFO << "test time is: " << distanceBreakTime << endl;
                //     double angleBbreakTime = getAngleBreakTime(dst->mobility->getSpeedXMean(),
                //                                                dst->mobility->getSpeedYMean(),
                //                                                dst->mobility->getSpeedXStdDev(),
                //                                                dst->mobility->getSpeedYStdDev(),
                //                                                dst->mobility->getCurrentPosition().getX(),
                //                                                dst->mobility->getCurrentPosition().getY(),
                //                                                newBeamwidth,
                //                                                dst->mobility->getUpdateInterval());

                //     breakTime = distanceBreakTime; // Ensure that breakTime has a value when loop ends.
                //     if (angleBbreakTime < distanceBreakTime)
                //     {
                //         breakTime = angleBbreakTime;
                //         break;
                //     }

                //     rangeMultiple--;
                // }

                double nextTime = simTime().dbl() + breakTime;
                txSchedule.insert({nextTime, dst});
            }
        }

        /* Print time table */
        EV_INFO << "txSchedule" << endl;
        EV_INFO << txSchedule << endl;

        /* Get refresh time from prediction */
        double refreshtime;
        if (!txSchedule.empty())
        {
            refreshtime = txSchedule.begin()->first;
            cancelEvent(txTimer);
            EV_INFO << "refreshtime: " << refreshtime << endl;
            scheduleAt(refreshtime, txTimer);
        }
    }
    else if (stage == 20)
    {
        mobility = dynamic_cast<inet::GaussMobility *>(getSubmodule("mobility"));
        pos = mobility->getCurrentPosition();
        // EV_INFO << "Cur pos is " << pos << endl;
        // scheduleAfter(transInterval, txTimer); // fixed refresh time

        subscribe(inet::IMobility::mobilityStateChangedSignal, this);
        EV_INFO << "subscribe(IMobility::mobilityStateChangedSignal, this)" << endl;

        scheduleAfter(10, powerMonitorTimer); // monitor the power per 10s
    }
    else
    {
        // EV_ERROR << "Unknown initialization stage. Error stage value is " << stage << endl;
    }
}

void SimpleNode::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
    if (msg == powerMonitorTimer)
    {
        scheduleAfter(10, powerMonitorTimer);
    }

    // cal listenning state energy consuming.
    simtime_t duration = simTime() - lastRecordTime;
    currentPower = basePower + listenningPower;
    consumption += duration.dbl() * currentPower;
    lastRecordTime = simTime();
    // EV_INFO << "Consumption is " << consumption << "now." << endl;

    /* Power off when the battery is exhausted. */
    if (consumption > batteryStorage_joule)
    {
        EV_INFO << getFullName() << ": No response. The battery is exhausted." << endl;

        // Do not reply any message
        if (cPacket *pkt = dynamic_cast<cPacket *>(msg))
        {
            delete pkt;
        }

        // Disconnect all links.
        for (NbrTable::iterator iter = neighborTable->begin(); iter != neighborTable->end();)
        {
            auto dst = check_and_cast<SimpleNode *>(iter->first);
            iter++;
            neighborTable->erase(dst);
        }

        return;
    }

    if (msg->isSelfMessage())
    {
        handleSelfMessage(msg);
    }
    else if (cPacket *pkt = dynamic_cast<cPacket *>(msg))
    {
        EV_INFO << "New packet attemptting, " << pkt << endl;
        if (Beacon *recvPkt = dynamic_cast<Beacon *>(pkt))
        {
            /* Received a maintain beacon. */

            auto *pktSrc = recvPkt->getSrc();
            auto *pktDst = recvPkt->getDst();
            EV_INFO << "Received a packet named " << recvPkt->getName() << endl;
            EV_INFO << "src id is " << recvPkt->getSrc()->getId() << endl;
            EV_INFO << "timestamp is " << recvPkt->getTimestamp() << endl;

            /* cal energy consuming for receiving a beacon. */
            currentPower = basePower + receivingPower;
            consumption += beaconDuration.dbl() * currentPower;

            // reply ack
            BeaconAck *pktAck = new BeaconAck("Ack for link maintain beacon");
            pktAck->setSrc(this);
            pktAck->setDst(const_cast<cModule *>(pktSrc));
            pktAck->setTimestamp(simTime());
            sendDirect(pktAck, const_cast<cModule *>(pktSrc)->gate("in"));

            /* cal energy consuming for sending ack. */
            currentPower = basePower + transmittingPower;
            consumption += beaconDuration.dbl() * currentPower;

            // free
            delete pkt;
        }
        else if (BeaconAck *ackPkt = dynamic_cast<BeaconAck *>(msg))
        {
            /* Received an ack for maintain beacon. */

            EV_INFO << "Received a packet named " << ackPkt->getName() << endl;
            EV_INFO << "src id is " << ackPkt->getSrc()->getId() << endl;
            EV_INFO << "timestamp is " << ackPkt->getTimestamp() << endl;

            /* Reset link state when received a ack. */
            auto maintainNode = const_cast<cModule *>(ackPkt->getSrc());

            /**
             * Refresh the direction vector in the neighbor table.
             * If the prediction method is adopted, there will be some observation errors.
             */

            /**
             * TODO:
             * 1. Change beamwidth.
             * 2. Get distance break probability
             * 3. Make angle break probability greater than distance break probability.
             * 4. Refresh direction vector with new communication range and beamwidth.
             */

            auto dir = check_and_cast<SimpleNode *>(maintainNode)->getPosition() - pos;
            (*neighborTable)[maintainNode] = dir.normalize() * (*neighborTable)[maintainNode].length(); // new direction and old communication range.

            /* Set next refresh time. */
            double breakTime;
            auto dst = check_and_cast<SimpleNode *>(maintainNode);

            breakTime = getBreakTime(dst); // new communication range is already set in getBreakTime().

            // breakTime = getAngleBreakTime(dst->mobility->getSpeedXMean(),
            //                               dst->mobility->getSpeedYMean(),
            //                               dst->mobility->getSpeedXStdDev(),
            //                               dst->mobility->getSpeedYStdDev(),
            //                               dst->mobility->getCurrentPosition().getX(),
            //                               dst->mobility->getCurrentPosition().getY(),
            //                               beamwidth,
            //                               dst->mobility->getUpdateInterval());
            txSchedule.insert({simTime().dbl() + breakTime, dst});

            /* Perhaps new fresh time is the earliest. */
            double refreshTime = txSchedule.begin()->first;
            EV_INFO << "Next time is " << refreshTime << endl;
            cancelEvent(txTimer);
            scheduleAt(refreshTime, txTimer);

            delete ackPkt;
        }
        else
        {
            throw cRuntimeError("Packet '%s' received on unexpectedly", msg->getName());
        }
    }
    else
    {
        throw cRuntimeError("Message '%s' received on unexpectedly", msg->getName());
    }
}

void SimpleNode::handleSelfMessage(cMessage *msg)
{
    if (msg == txTimer)
    {
        /**
         * check ack
         * ack is valid: send beacon
         * send beacon by "sendToNode()"
         * ack is unvalid : neighbor is lost. Delete.
         *
         */
        EV_INFO << "handle [txTimer] msg." << endl;

        if ((*neighborTable).empty())
        {
            /* No neighbor in table. Stay alive for ack. */
            EV_ERROR << "NeighborTable is empty." << endl;
            return;
        }

        auto dst = check_and_cast<SimpleNode *>(txSchedule.begin()->second);
        txSchedule.erase(txSchedule.begin());

        if ((*neighborTable).find(dst) != (*neighborTable).end())
        {
            /* Still in neighbor table */
            emit(sendCountSignal, 1);

            /* send */
            if (canSendToNode(dst))
            {
                Beacon *pkt = new Beacon("link maintain beacon");
                pkt->setSrc(this);
                pkt->setDst(dst);
                pkt->setTimestamp(simTime());
                sendDirect(pkt, dst->gate("in"));
            }
            else
            {
                EV_INFO << "cant send to node " << dst->getFullName() << endl;
                txSchedule.insert({simTime().dbl() + 2.0, dst});
            }

            /* cal energy consuming for sending a beacon. */
            currentPower = basePower + transmittingPower;
            consumption += beaconDuration.dbl() * currentPower;
        }

        if (!txSchedule.empty())
        {
            double refreshTime = txSchedule.begin()->first;
            cancelEvent(txTimer);
            scheduleAt(refreshTime, txTimer);
        }
    }
}

void SimpleNode::receiveSignal(cComponent *source, simsignal_t signalID, cObject *object, cObject *details)
{
    if (signalID == inet::IMobility::mobilityStateChangedSignal)
    {
        if (auto mobility = dynamic_cast<inet::IMobility *>(source))
        {
            EV_INFO << "Received a signal named mobilityStateChangedSignal." << endl;
            EV_INFO << mobility->getCurrentPosition() << endl;
            pos = mobility->getCurrentPosition();
        }
        EV_INFO << getFullName() << "pos is " << pos << endl;
    }
    else
    {
        EV_INFO << "Unknown Signal." << endl;
    }
}

bool SimpleNode::canSendToNode(SimpleNode *dst)
{
    auto dis_v = dst->getPosition() - pos;
    auto directionVector = (*neighborTable)[dst];
    EV << (*neighborTable) << endl;
    auto ang = dis_v.angle(directionVector);

    EV_INFO << dst->getFullName() << endl;
    EV_INFO << "normal vector is " << directionVector << endl;
    EV_INFO << "dis_v is " << dis_v << endl;
    EV_INFO << "dis_v normal is " << dis_v.normalize() << endl;
    EV_INFO << "ang is " << ang << endl;
    EV_INFO << "distance is " << pos.distance(dst->getPosition()) << endl;

    return ((ang < (beamwidth / 2)) &&
            (pos.distance(dst->getPosition()) < directionVector.length()));
}

void SimpleNode::finish()
{
    EV_INFO << getFullName() << " Finish() " << endl;
    EV_INFO << (*neighborTable) << endl;
    EV_INFO << "consumption is " << consumption << endl;
}

std::ostream &operator<<(std::ostream &os, const LinkState &ls)
{
    for (auto const &subMap : ls)
    {
        os << "------ " << subMap.first->getFullName() << " ------" << endl;
        os << subMap.second << endl;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const TimeTable &tt)
{
    for (auto const &subMap : tt)
    {
        os << "------ " << subMap.second->getFullName() << " ------" << endl;
        os << subMap.first << endl;
    }
    return os;
}

/******************* Angle Breakage Probability Prediction Function *******************/
double SimpleNode::calculate_angle(const std::array<double, 2> &vec1, const std::array<double, 2> &vec2)
{
    double dot_product = vec1[0] * vec2[0] + vec1[1] * vec2[1];
    double norm1 = std::sqrt(vec1[0] * vec1[0] + vec1[1] * vec1[1]);
    double norm2 = std::sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1]);
    double cos_theta = dot_product / (norm1 * norm2);
    return std::acos(cos_theta);
}

double SimpleNode::getAngleBreakTime(double vx_mean, double vy_mean,
                                     double vx_delta, double vy_delta,
                                     double x0, double y0,
                                     double bw, double dt, int loop)
{
    double simtime = 1000 * dt;

    /* * calculate relative speed
     * Assume the velocities of nodes X and Y are random variables v_X and v_Y, respectively:
     * - v_X ~ N(mu_X, sigma_X^2)
     * - v_Y ~ N(mu_Y, sigma_Y^2)
     * - v_X and v_Y are independent
     *
     * In the reference frame of node X, the velocity of node Y, v_Y|X, satisfies:
     * - v_Y|X = v_Y - v_X
     * - Mean: mu_Y|X = mu_Y - mu_X
     * - Variance: sigma_Y|X^2 = sigma_Y^2 + sigma_X^2
     *
     * Therefore:
     * v_Y|X ~ N(mu_Y - mu_X, sigma_Y^2 + sigma_X^2)
     */
    vx_mean = vx_mean - mobility->getSpeedXMean();
    vy_mean = vy_mean - mobility->getSpeedYMean();

    vx_delta = sqrt(vx_delta * vx_delta + mobility->getSpeedXStdDev() * mobility->getSpeedXStdDev());
    vy_delta = sqrt(vy_delta * vy_delta + mobility->getSpeedYStdDev() * mobility->getSpeedYStdDev());

    x0 = x0 - pos.getX();
    y0 = y0 - pos.getY();

    /* Rotate the coordinate system to ensure y0 is 0. */
    double rotationAngle = atan2(y0, x0);
    auto [vx_mean_rotated, vy_mean_rotated, vx_delta_rotated, vy_delta_rotated] = rotateVelocity(vx_mean, vy_mean, vx_delta, vy_delta, rotationAngle);
    x0 = sqrt(x0 * x0 + y0 * y0);
    y0 = 0;

    std::vector<int> result;

    for (int i = 0; i < loop; ++i)
    {
        int steps = 0;
        double t = 0;
        std::array<double, 2> n1Posi = {x0, y0};
        std::array<double, 2> n2Posi = {0, 0};

        while (t < simtime)
        {
            // Generate speed randomly
            double v_x = normal(vx_mean_rotated, vx_delta_rotated);
            double v_y = normal(vy_mean_rotated, vy_delta_rotated);

            // update position
            n1Posi[0] += v_x * dt;
            n1Posi[1] += v_y * dt;

            // calculate angle
            double ang = calculate_angle(n1Posi, {1, 0});

            // check angle
            if (ang > bw / 2.0)
            {
                break;
            }

            t += dt;
            steps++;
        }
        result.push_back(steps);
    }

    double sum = std::accumulate(std::begin(result), std::end(result), 0.0);
    double mean = sum / result.size(); // 均值
    auto minValue = std::min_element(result.begin(), result.end());
    EV_INFO << "min value is    " << *minValue << endl;

    // double nextTime = (mean)*dt;
    double nextTime = (*minValue) * dt;

    return nextTime;
}

std::tuple<double, double, double, double> SimpleNode::rotateVelocity(
    double vx_mean, double vy_mean,
    double vx_sigma, double vy_sigma,
    double theta)
{
    // Compute the cosine and sine of the rotation angle
    double cos_theta = std::cos(theta);
    double sin_theta = std::sin(theta);

    // Compute the new means
    double vx_new_mean = vx_mean * cos_theta + vy_mean * sin_theta;
    double vy_new_mean = -vx_mean * sin_theta + vy_mean * cos_theta;

    // Compute the new variances
    double vx_new_var = (vx_sigma * vx_sigma) * (cos_theta * cos_theta) + (vy_sigma * vy_sigma) * (sin_theta * sin_theta);
    double vy_new_var = (vx_sigma * vx_sigma) * (sin_theta * sin_theta) + (vy_sigma * vy_sigma) * (cos_theta * cos_theta);

    // Standard deviation is the square root of variance
    double vx_new_sigma = std::sqrt(vx_new_var);
    double vy_new_sigma = std::sqrt(vy_new_var);

    // Return the results
    return {vx_new_mean, vy_new_mean, vx_new_sigma, vy_new_sigma};
}

/******************* Distance Breakage Probability Prediction Function *******************/
double SimpleNode::getDistanceBreakTime(double vx_mean, double vy_mean,
                                        double vx_delta, double vy_delta,
                                        double x0, double y0,
                                        double R, double dt, int loop)
{
    double simtime = 1000 * dt;

    int loopCnt = 10;
    int minSteps = 10000;

    /* * calculate relative speed
     * Assume the velocities of nodes X and Y are random variables v_X and v_Y, respectively:
     * - v_X ~ N(mu_X, sigma_X^2)
     * - v_Y ~ N(mu_Y, sigma_Y^2)
     * - v_X and v_Y are independent
     *
     * In the reference frame of node X, the velocity of node Y, v_Y|X, satisfies:
     * - v_Y|X = v_Y - v_X
     * - Mean: mu_Y|X = mu_Y - mu_X
     * - Variance: sigma_Y|X^2 = sigma_Y^2 + sigma_X^2
     *
     * Therefore:
     * v_Y|X ~ N(mu_Y - mu_X, sigma_Y^2 + sigma_X^2)
     */
    vx_mean = vx_mean - mobility->getSpeedXMean();
    vy_mean = vy_mean - mobility->getSpeedYMean();

    vx_delta = sqrt(vx_delta * vx_delta + mobility->getSpeedXStdDev() * mobility->getSpeedXStdDev());
    vy_delta = sqrt(vy_delta * vy_delta + mobility->getSpeedYStdDev() * mobility->getSpeedYStdDev());

    x0 = x0 - pos.getX();
    y0 = y0 - pos.getY();

    EV_INFO << "------------------------------------" << endl;
    EV_INFO << "vx_mean:    " << vx_mean << endl;
    EV_INFO << "vy_mean:    " << vy_mean << endl;
    EV_INFO << "vx_delta:   " << vx_delta << endl;
    EV_INFO << "vy_delta:   " << vy_delta << endl;
    EV_INFO << "vx_mean:    " << vx_mean << endl;
    EV_INFO << "x0:         " << x0 << endl;
    EV_INFO << "y0:         " << y0 << endl;
    EV_INFO << "R:          " << R << endl;
    EV_INFO << "dt:         " << dt << endl;
    EV_INFO << "loop:       " << loop << endl;

    /* Rotate the coordinate system to ensure y0 is 0. */

    double sum = 0.0;
    for (int i = 0; i < loopCnt; i++)
    {
        for (int j = 0; j < loop; ++j)
        {
            int steps = 0;
            double t = 0;
            std::array<double, 2> n1Posi = {x0, y0};
            std::array<double, 2> n2Posi = {0, 0};

            while (t < simtime)
            {
                // Generate speed randomly
                double v_x = normal(vx_mean, vx_delta);
                double v_y = normal(vy_mean, vy_delta);

                // update position
                n1Posi[0] += v_x * dt;
                n1Posi[1] += v_y * dt;

                // calculate angle
                double distance = sqrt(n1Posi[0] * n1Posi[0] + n1Posi[1] * n1Posi[1]);

                // check angle
                if (distance > R)
                {
                    break;
                }

                t += dt;
                steps++;
            }
            minSteps = minSteps > steps ? steps : minSteps;
        }
        sum += minSteps * 1.0;
    }
    double mean = sum / loopCnt;
    double nextTime = (mean)*dt;

    return nextTime;
}

/******************* Breakage Probability Prediction Function *******************/
double SimpleNode::getBreakTime(SimpleNode *dst)
{
    double breakTime;

    if (!isPred)
    {
        return transInterval;
    }

    EV_INFO << "dst x pos: " << dst->mobility->getCurrentPosition().getX() << endl;
    EV_INFO << "dst y pos: " << dst->mobility->getCurrentPosition().getY() << endl;

    // int rangeMultiple = 0;
    int rangeMultiple = ((*neighborTable)[dst].length() - commRange) / commRangeIncr;
    rangeMultiple = rangeMultiple < rangeMultipleMax ? rangeMultiple : (rangeMultipleMax - 1);
    EV_INFO << "rangeMultiple = " << rangeMultiple << endl;
    double newCommRange = commRange;
    while (rangeMultiple < rangeMultipleMax)
    {
        // newCommRange = (*neighborTable)[dst].length() + rangeMultiple * commRangeIncr;
        newCommRange = commRange + rangeMultiple * commRangeIncr;
        double newBeamwidth = getBeamwidthFromCommRange(newCommRange);
        EV_INFO << "rangeMultiple is   " << rangeMultiple << endl;
        double distanceBreakTime = getDistanceBreakTime(dst->mobility->getSpeedXMean(),
                                                        dst->mobility->getSpeedYMean(),
                                                        dst->mobility->getSpeedXStdDev(),
                                                        dst->mobility->getSpeedYStdDev(),
                                                        dst->mobility->getCurrentPosition().getX(),
                                                        dst->mobility->getCurrentPosition().getY(),
                                                        newCommRange,
                                                        dst->mobility->getUpdateInterval());

        EV_INFO << "distanceBreakTime is: " << distanceBreakTime << endl;
        double angleBbreakTime = getAngleBreakTime(dst->mobility->getSpeedXMean(),
                                                   dst->mobility->getSpeedYMean(),
                                                   dst->mobility->getSpeedXStdDev(),
                                                   dst->mobility->getSpeedYStdDev(),
                                                   dst->mobility->getCurrentPosition().getX(),
                                                   dst->mobility->getCurrentPosition().getY(),
                                                   newBeamwidth,
                                                   dst->mobility->getUpdateInterval());
        EV_INFO << "angleBbreakTime is: " << angleBbreakTime << endl;
        breakTime = distanceBreakTime == 0 ? 0.1 : distanceBreakTime; // Ensure that breakTime has a value when loop ends. 0.1 ensures that do not enter a infinite loop.
        if (angleBbreakTime < distanceBreakTime)
        {
            breakTime = angleBbreakTime > 0 ? angleBbreakTime : 0.1;
            break;
        }

        rangeMultiple++;
    }
    (*neighborTable)[dst] = (*neighborTable)[dst].normalize() * newCommRange; // new communication range
    return breakTime;
}

double SimpleNode::getBeamwidthFromCommRange(double newCommRange)
{
    double newBeamwidth = beamwidth / (newCommRange / commRange);
    // EV_INFO << "newCommRange:   " << newCommRange << endl;
    // EV_INFO << "commRange:      " << commRange << endl;
    // EV_INFO << "newBeamwidth:   " << newBeamwidth << endl;
    // EV_INFO << "beamwidth:      " << beamwidth << endl;
    return newBeamwidth;
}