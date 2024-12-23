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

        /**
         * Energy(J) = Battery Capacity(mAh) / 1000 * Voltage(V) * 3600
         */
        batteryStorage_joule = batteryStorage / 1000 * nominalVoltage * 3600;
        EV_INFO << "batteryStorage_joule = " << batteryStorage_joule << endl;

        txTimer = new cMessage("txTimer");
        isDirectionalReceive = false;

        powerMonitorTimer = new cMessage("powerMonitorTimer");

        txQueue = new cQueue("send pkt queue");

        linkStateThreshold = par("linkStateThreshold");

        beamwidth = 2 * M_PI / numSectors;

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
    else if (stage == 22)
    {
        // get nbr information
        neighborTable = radioMedium->getNeighborTable(this);
        if (!(*neighborTable).empty())
        {
            EV_INFO << "---------Node " << nodeId << " Neighbor Table--------" << endl;
            EV_INFO << (*neighborTable);

            /* Init link state. */
            for (NbrTable::iterator iter = (*neighborTable).begin();
                 iter != (*neighborTable).end();
                 iter++)
            {
                linkState[iter->first] = linkStateThreshold;
            }
            EV_INFO << linkState << endl;
            subscribe(linkEndSignal, radioMedium);
        }
        // scheduleAfter(2, txTimer);
        // subscribe(IMobility::mobilityStateChangedSignal, this);
    }
    else if (stage == 20)
    {
        auto mobility = dynamic_cast<inet::IMobility *>(getSubmodule("mobility"));
        pos = mobility->getCurrentPosition();
        // EV_INFO << "Cur pos is " << pos << endl;
        scheduleAfter(transInterval, txTimer);
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
            radioMedium->linkLifetimeNotice(simTime());
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
            linkState[maintainNode] = linkStateThreshold;

            /**
             * Refresh the direction vector in the neighbor table.
             * If the prediction method is adopted, there will be some observation errors.
             */

            auto dir = check_and_cast<SimpleNode *>(maintainNode)->getPosition() - pos;
            (*neighborTable)[maintainNode] = dir.normalize();

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
            return;
        }

        for (NbrTable::iterator iter = (*neighborTable).begin();
             iter != (*neighborTable).end();)
        {
            auto dst = check_and_cast<SimpleNode *>(iter->first);

            /* change link state */
            if (linkState[dst] < 0)
            {
                iter++;
                linkState.erase(dst);
                (*neighborTable).erase(dst);

                radioMedium->linkLifetimeNotice(simTime());
                continue;
            }
            else
            {
                iter++;
                linkState[dst]--;
                // statistic
                emit(sendCountSignal, 1);
            }

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
            }

            /* cal energy consuming for sending a beacon. */
            currentPower = basePower + transmittingPower;
            consumption += beaconDuration.dbl() * currentPower;
        }

        EV_INFO << "Map status after countdown." << endl;
        EV_INFO << (*neighborTable) << endl;
        EV_INFO << linkState << endl;

        if ((*neighborTable).empty())
        {
            // radioMedium->lostNodeNotice(this);
        }
        else
        {
            scheduleAfter(transInterval, txTimer);
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
    auto ang = dis_v.angle((*neighborTable)[dst]);

    EV_INFO << dst->getFullName() << endl;
    EV_INFO << "normal vector is " << (*neighborTable)[dst] << endl;
    EV_INFO << "dis_v is " << dis_v << endl;
    EV_INFO << "dis_v normal is " << dis_v.normalize() << endl;
    EV_INFO << "ang is " << ang << endl;
    EV_INFO << "distance is " << pos.distance(dst->getPosition()) << endl;

    return ((ang < (beamwidth / 2)) &&
            (pos.distance(dst->getPosition()) < commRange));
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
