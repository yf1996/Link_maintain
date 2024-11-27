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

#include "RadioMedium.h"

#include "SimpleNode.h"
#include "inet/mobility/base/MobilityBase.h"

#define PI 3.14159265358979323846

using namespace omnetpp;

Define_Module(RadioMedium);

simsignal_t RadioMedium::currentLinkNumSignal = registerSignal("currentLinkNum");
simsignal_t RadioMedium::lostNodeSignal = registerSignal("lostNode");

const double RadioMedium::propagationSpeed = 299792458.0;

RadioMedium::RadioMedium() : constraintAreaMinX(0.0),
                             constraintAreaMinY(0.0),
                             constraintAreaMinZ(0.0),
                             constraintAreaMaxX(0.0),
                             constraintAreaMaxY(0.0),
                             constraintAreaMaxZ(0.0)
{
}

RadioMedium::~RadioMedium()
{
}

void RadioMedium::registerNode(cModule *SimpleNode)
{
    Enter_Method("RadioMedium::registerNode()");
    nodeSet.insert(SimpleNode);
    EV_INFO << "Registered " << SimpleNode->getFullName() << endl;
}

void RadioMedium::startBeaconTransmition(cModule *src)
{
    Enter_Method("Scenario::startBeaconTransmition");
    //    sendDirectToAllNode(src);
    //    sendToAffectedNode(src);
}

void RadioMedium::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        constraintAreaMinX = par("constraintAreaMinX");
        constraintAreaMinY = par("constraintAreaMinY");
        constraintAreaMinZ = par("constraintAreaMinZ");
        constraintAreaMaxX = par("constraintAreaMaxX");
        constraintAreaMaxY = par("constraintAreaMaxY");
        constraintAreaMaxZ = par("constraintAreaMaxZ");

        monitorTimer = new cMessage("Monitor Timer");
        monitorInterval = 0.1;
    }
    else if (stage == INITSTAGE_MOBILITY)
    {
    }
    else if (stage == INITSTAGE_ENVIRONMENT)
    {
        EV_INFO << nodeSet << endl;
    }
    else if (stage == 21)
    {
        refreshNeighborInfo();
        emit(currentLinkNumSignal, GetCurrentLinkNum());
        emit(lostNodeSignal, GetCurrentNodeNum());

        scheduleAfter(monitorInterval - 0.01, monitorTimer);
    }
    else if (stage == INITSTAGE_LAST)
    {
    }
    else
    {
        EV_ERROR << "Unknown initialization stage. Error stage value is " << stage << endl;
    }
}

void RadioMedium::refreshNeighborInfo()
{
    nbrInfo.clear();
    for (auto i : nodeSet)
    {
        auto src = check_and_cast<SimpleNode *>(i);
        NbrTable &subTable = nbrInfo[i];
        for (auto j : nodeSet)
        {
            if (i == j)
            {
                continue;
            }
            auto dst = check_and_cast<SimpleNode *>(j);
            inet::Coord dir = dst->getPosition() - src->getPosition();
            if (dir.length() > src->getCommunicationRange())
            {
                continue;
            }

            subTable[j] = dir.normalize() * src->getCommunicationRange();
        }
    }
    EV_INFO << nbrInfo;
}

void RadioMedium::linkLifetimeNotice(simtime_t linkEndTime)
{
    Enter_Method("RadioMedium::linkLifetimeNotice()");
    EV_INFO << "linkLifetimeNotice" << endl;

    emit(currentLinkNumSignal, GetCurrentLinkNum());
    emit(lostNodeSignal, GetCurrentNodeNum());
}

void RadioMedium::lostNodeNotice(cModule *lostNode)
{
    Enter_Method("RadioMedium::lostNodeNotice()");
    EV_INFO << "lostNodeNotice" << endl;
    // emit(lostNodeSignal, 1);
}

NbrTable *RadioMedium::getNeighborTable(cModule *src)
{
    auto s = check_and_cast<SimpleNode *>(src);

    if (nbrInfo.find(s) != nbrInfo.end())
    {
        return &nbrInfo[s];
    }
    else

    {
        EV_INFO << "SimpleNode " << s->getId() << " is not in nbrInfo." << endl;
        NbrTable blank;
        // return blank;
        return nullptr;
    }
}

void RadioMedium::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
    if (msg == monitorTimer)
    {
        /* refresh link number. */
        int onlineLinkNum = 0;
        int onlineNodeNum = 0;
        if (!nbrInfo.empty())
        {
            for (NbrMap::iterator nbrMapIter = nbrInfo.begin();
                 nbrMapIter != nbrInfo.end();)
            {
                if (!nbrMapIter->second.empty())
                {
                    onlineNodeNum++;

                    for (NbrTable::iterator nbrTableIter = nbrMapIter->second.begin();
                         nbrTableIter != nbrMapIter->second.end();)
                    {
                        /* check if link vector is valid. */
                        auto src = check_and_cast<SimpleNode *>(nbrMapIter->first);
                        auto dst = check_and_cast<SimpleNode *>(nbrTableIter->first);
                        auto directionVector = nbrTableIter->second;

                        auto posVector = dst->getPosition() - src->getPosition();
                        double ang = directionVector.getNormalized() == posVector.normalize() ? 0 : directionVector.angle(posVector);
                        double distance = posVector.length();
                        EV_INFO << "ang is " << ang << endl;
                        EV_INFO << "directionVector is " << directionVector << endl;
                        EV_INFO << "posVector is " << posVector / posVector.length() << endl;
                        EV_INFO << "commRange:      " << directionVector.length() << endl;
                        EV_INFO << "distance:      " << distance << endl;

                        nbrTableIter++;

                        /* Beamwidth varies with communication range. */
                        auto halfBw = src->getBeamwidthFromCommRange(directionVector.length()) / 2;
                        EV_INFO << "halfBw is " << halfBw << endl;

                        if (ang < halfBw && distance < directionVector.length())
                        {
                            onlineLinkNum++;
                        }
                        else
                        {
                            EV_INFO << "earse(dst)" << endl;
                            nbrMapIter->second.erase(dst);
                        }
                    }
                }
                else
                {
                    EV_ERROR << "nbrtable is empty. " << endl;
                }

                nbrMapIter++;
            }
        }
        else
        {
            EV_ERROR << "nbrInfo is empty. " << endl;
        }

        // emit statistic signal
        emit(currentLinkNumSignal, onlineLinkNum);
        emit(lostNodeSignal, onlineNodeNum);

        scheduleAfter(monitorInterval, monitorTimer); // periodic
    }
    else
    {
        /* code */
        throw cRuntimeError("Not Implemented");
    }
}

void RadioMedium::receiveSignal(cComponent *source, simsignal_t signalID, const SimTime &t, cObject *details)
{
    if (signalID == SimpleNode::linkEndSignal)
    {
        /* code */
        emit(currentLinkNumSignal, t);
    }
}

int RadioMedium::GetCurrentLinkNum()
{
    int linkCnt = 0;
    if (!nbrInfo.empty())
    {
        NbrMap::iterator iter = nbrInfo.begin();
        while (iter != nbrInfo.end())
        {
            // auto tableOfNode = check_and_cast<NbrTable *>(&(iter->second));
            auto tableOfNode = iter->second;
            linkCnt += tableOfNode.size();
            iter++;
        }
    }
    else
    {
        EV_WARN << "GetCurrentLinkNum: nbrInfo in radioMedium is empty!" << endl;
    }
    return linkCnt;
}

int RadioMedium::GetCurrentNodeNum()
{
    int nodeCnt = 0;
    if (!nbrInfo.empty())
    {
        NbrMap::iterator iter = nbrInfo.begin();
        while (iter != nbrInfo.end())
        {
            auto tableOfNode = iter->second;
            nodeCnt += tableOfNode.empty() ? 0 : 1;
            iter++;
        }
    }
    else
    {
        EV_WARN << "GetCurrentNodeNum: nbrInfo in radioMedium is empty!" << endl;
    }
    return nodeCnt;
}

inet::Coord RadioMedium::genRandomConstraintAreaPos()
{
    return inet::Coord(uniform(constraintAreaMinX, constraintAreaMaxX),
                       uniform(constraintAreaMinY, constraintAreaMaxY),
                       uniform(constraintAreaMinZ, constraintAreaMaxZ));
}

inline std::ostream &operator<<(std::ostream &os, const NodeSet &nbrSet)
{
    for (auto const &entry : nbrSet)
    {
        os << entry->getFullName() << " ";
    }
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const NbrTable &nbrTbl)
{
    for (auto const &subTbl : nbrTbl)
    {
        os << "[" << subTbl.first << "]: ";
        os << subTbl.second << endl;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const NbrMap &nbrMap)
{
    for (auto const &subMap : nbrMap)
    {
        os << "------ " << subMap.first->getFullName() << " ------" << endl;
        os << subMap.second << endl;
    }
    return os;
}

void RadioMedium::finish()
{
    EV_INFO << "RadioMedium Finish():" << endl;
    EV_INFO << nbrInfo << endl;
}
