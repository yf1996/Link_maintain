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

#ifndef __LINK_MAINTAIN_RADIOMEDIUM_H_
#define __LINK_MAINTAIN_RADIOMEDIUM_H_

#include <map>
#include <omnetpp.h>
#include <set>
#include <vector>

#include "inet/common/geometry/common/Coord.h"
#include "param_setting.h"

using namespace omnetpp;
using SecId = int;

/**
 * @brief Including direction information and communication distance information.
 * Each node has a special communication distance.
 * The beamwidth should vary with the communication distance. 
 */
using OrientVector = inet::Coord;
 
using NodeSet = std::set<cModule *>;
using NbrTable = std::map<cModule *, OrientVector>;
using NbrMap = std::map<cModule *, NbrTable>;

/**
 * TODO - Generated class
 */
class RadioMedium : public cSimpleModule, public cListener
{
protected:
    NodeSet nodeSet; // All nodes in this medium.
    /**
     * @brief The infomation between all nodes.
     *
     * nbrInfo = <nodeId, neighbor table<id of sector, neighbors set> >
     */
    NbrMap nbrInfo;

    double constraintAreaMinX;
    double constraintAreaMinY;
    double constraintAreaMinZ;
    double constraintAreaMaxX;
    double constraintAreaMaxY;
    double constraintAreaMaxZ;

    cMessage *monitorTimer = nullptr;
    double monitorInterval = 0;

public:
    static const double propagationSpeed;

    static simsignal_t currentLinkNumSignal;
    static simsignal_t lostNodeSignal;

public:
    RadioMedium();
    virtual ~RadioMedium();
    virtual void registerNode(cModule *node);
    virtual inet::Coord genRandomConstraintAreaPos();
    virtual void startBeaconTransmition(cModule *src);

    virtual const NodeSet &getNodeSet()
    {
        return nodeSet;
    }
    /**
     * @brief Update the neighbors information of all nodes.
     * Record the sector ID the neighbor is in when src sends.
     */
    virtual void refreshNeighborInfo();

    virtual NbrTable *getNeighborTable(cModule *src);

    void linkLifetimeNotice(simtime_t linkEndTime);
    void lostNodeNotice(cModule *lostNode);
    /**
     * @brief Get the Current Link Num. 
     * 
     * Link A->B is different to Link B->A. 
     * 
     * @return int 
     */
    int GetCurrentLinkNum();

    /**
     * @brief Get the Current Node Num.
     * 
     * Calculate the node that its neighbor table is not empty.
     * 
     * @return int 
     */
    int GetCurrentNodeNum();

private:
    /** @brief Copy constructor is not allowed. */
    RadioMedium(const RadioMedium &);
    /** @brief Assignment operator is not allowed. */
    RadioMedium &operator=(const RadioMedium &);

protected:
    /** @brief Get the number of total initialization stages. */
    virtual int numInitStages() const override
    {
        return NUM_INIT_STAGES;
    }
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const SimTime &t, cObject *details) override;
    virtual void finish() override;

    /**
     * @brief Initiate transmission to all nodes, without check link validity.
     */
    // virtual void sendDirectToAllNode(cModule *src);

    /**
     * @brief Initiate trasnmission attempts to nodes in current working antenna sector.
     */
    //    virtual void sendToAffectedNode(cModule *src);

    /**
     * @brief Hanle transmission attempts, initiate transmission if link exists.
     */
    //    virtual void sendToNode(cModule *src, cModule *dst);
};

inline std::ostream &operator<<(std::ostream &os, const NodeSet &nbrSet);
inline std::ostream &operator<<(std::ostream &os, const NbrTable &nbrTbl);
inline std::ostream &operator<<(std::ostream &os, const NbrMap &nbrMap);

#endif
