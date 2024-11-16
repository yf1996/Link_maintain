//
// Generated file, do not edit! Created by opp_msgtool 6.0 from beacon_ack.msg.
//

#ifndef __BEACON_ACK_M_H
#define __BEACON_ACK_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// opp_msgtool version check
#define MSGC_VERSION 0x0600
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgtool: 'make clean' should help.
#endif

class BeaconAck;
/**
 * Class generated from <tt>beacon_ack.msg:19</tt> by opp_msgtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * packet BeaconAck
 * {
 *     cModule *src;
 *     cModule *dst;
 *     simtime_t timestamp;
 * }
 * </pre>
 */
class BeaconAck : public ::omnetpp::cPacket
{
  protected:
    omnetpp::cModule * src = nullptr;
    omnetpp::cModule * dst = nullptr;
    omnetpp::simtime_t timestamp = SIMTIME_ZERO;

  private:
    void copy(const BeaconAck& other);

  protected:
    bool operator==(const BeaconAck&) = delete;

  public:
    BeaconAck(const char *name=nullptr, short kind=0);
    BeaconAck(const BeaconAck& other);
    virtual ~BeaconAck();
    BeaconAck& operator=(const BeaconAck& other);
    virtual BeaconAck *dup() const override {return new BeaconAck(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual const omnetpp::cModule * getSrc() const;
    virtual omnetpp::cModule * getSrcForUpdate() { return const_cast<omnetpp::cModule *>(const_cast<BeaconAck*>(this)->getSrc());}
    virtual void setSrc(omnetpp::cModule * src);

    virtual const omnetpp::cModule * getDst() const;
    virtual omnetpp::cModule * getDstForUpdate() { return const_cast<omnetpp::cModule *>(const_cast<BeaconAck*>(this)->getDst());}
    virtual void setDst(omnetpp::cModule * dst);

    virtual omnetpp::simtime_t getTimestamp() const;
    virtual void setTimestamp(omnetpp::simtime_t timestamp);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const BeaconAck& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, BeaconAck& obj) {obj.parsimUnpack(b);}


namespace omnetpp {

template<> inline BeaconAck *fromAnyPtr(any_ptr ptr) { return check_and_cast<BeaconAck*>(ptr.get<cObject>()); }

}  // namespace omnetpp

#endif // ifndef __BEACON_ACK_M_H

