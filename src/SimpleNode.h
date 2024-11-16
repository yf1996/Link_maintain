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

#ifndef __LINK_MAINTAIN_SIMPLENODE_H_
#define __LINK_MAINTAIN_SIMPLENODE_H_

#include <omnetpp.h>

#include "RadioMedium.h"
#include "beacon_ack_m.h"
#include "beacon_m.h"
#include "param_setting.h"

// #include "inet/power/base/PowerDefs.h"

using namespace omnetpp;

using SecId = int;
using W = double;
using J = double;
using CountDown = int;

using LinkState = std::map<cModule *, CountDown>;

/**
 * TODO - Generated class
 */
class SimpleNode : public cSimpleModule, public cListener
{
protected:
    /** @brief Get the number of total initialization stages. */
    virtual int numInitStages() const override
    {
        return NUM_INIT_STAGES;
    }
    /** @brief Initialize the node. */
    virtual void initialize(int stage) override;
    /** @brief Initialize the discovery process. */
    // virtual void initPosition();

    virtual void handleMessage(cMessage *msg) override;

    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *object, cObject *details) override;

    virtual void finish() override;

private:
    // Fixed parameters
    int nodeId = -1;         // Unique ID for the node
    int numSectors = -1;     // Number of sectors for communication
    double beamwidth = 0;    // unit:degree
    double commRange = -1.0; // Communication range of the node
                             // power comsumption parameters
    double transInterval = -1;
    W listenningPower = W(NaN);   ///< Power consumption while listening
    W receivingPower = W(NaN);    ///< Power consumption while receiving
    W transmittingPower = W(NaN); ///< Power consumption while transmitting
    W currentPower = W(0.0);      ///< Current power consumption
    J consumption = J(0.0);

    CountDown linkStateThreshold; // minus 1 when send. Reset when received ack.

    simtime_t beaconDuration = SIMTIME_ZERO; // Duration of a beacon
    simtime_t slotDuration = SIMTIME_ZERO;   // Duration of a slot
    RadioMedium *radioMedium = nullptr;      // Pointer to the radio medium
    bool isDirectionalReceive;
    NbrTable *neighborTable = nullptr;
    cQueue *txQueue = nullptr;
    LinkState linkState;

    cMessage *txTimer = nullptr;

    // Dynamic parameters
    simtime_t sendIntv = SIMTIME_ZERO;  ///< Interval for sending
    inet::Coord pos = inet::Coord::NIL; ///< Position of the node

    simtime_t lastRecordTime = 0;

public:
    static simsignal_t linkEndSignal;

public:
    /** @brief Get the node ID. */
    virtual int getNodeId() const
    {
        return nodeId;
    }

    /** @brief Get the slot duration. */
    virtual const simtime_t &getSlotDuration() const
    {
        return slotDuration;
    }

    /** @brief Get the position of the node. */
    virtual const inet::Coord &getPosition() const
    {
        return pos;
    }

    /** @brief Get the number of sectors. */
    virtual int getNumSectors() const
    {
        return numSectors;
    }

    /**  @brief Get the communication range. */
    virtual double getCommunicationRange() const
    {
        return commRange;
    }
    /**  @brief Get the received mode. */
    virtual bool isDirectionalReceived() const
    {
        return isDirectionalReceive;
    }
    /**
     * @brief Send a beacon to a specific node.
     *
     * @param dst Destination node.
     * @param delay Delay before sending the beacon.
     */
    //    virtual void sendBeaconToNode(cModule *dst, const simtime_t &delay);

protected:
    /** @brief Handle self messages. */
    virtual void handleSelfMessage(cMessage *msg);
    /** @brief Handle events with the FSM. */
    // virtual void handleMsgInFsm(cMessage *msg);
    /** @brief Cast a neighbor discovery beacon. */
    //    virtual void castBeacon();

    bool canSendToNode(SimpleNode *dst);
};

std::ostream &operator<<(std::ostream &os, const LinkState &ls);

#endif
