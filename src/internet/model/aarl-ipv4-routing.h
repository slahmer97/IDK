//
// Created by slahmer on 3/1/22.
//

#ifndef NS3_AARLIPV4ROUTING_H
#define NS3_AARLIPV4ROUTING_H
#include <memory>
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/ipv4-route.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv4-address.h"
#include "ipv4-global-routing.h"
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/uinteger.h"
#include <iostream>
#include <random>
using namespace std;
namespace ns3{

class MyTag : public Tag
{
public:
  inline MyTag():
        m_leaf_id(99999),
        m_interface(99999)
  {}
  inline static TypeId GetTypeId (){
    static TypeId tid = TypeId ("ns3::MyTag")
                            .SetParent<Tag> ()
                            .AddConstructor<MyTag> ()
                            .AddAttribute ("LeafId",
                                           "Identify the first leaf router id",
                                           EmptyAttributeValue (),
                                           MakeUintegerAccessor (&MyTag::GetLeafId),
                                           MakeUintegerChecker<uint32_t> ())
                            .AddAttribute ("InterfaceId",
                                           "Identify the interface id in the leaf switch",
                                           EmptyAttributeValue (),
                                           MakeUintegerAccessor (&MyTag::GetInterfaceId),
                                           MakeUintegerChecker<uint32_t> ())
        ;
    return tid;
  }
  inline virtual TypeId GetInstanceTypeId () const{
    return GetTypeId ();
  }
  inline virtual uint32_t GetSerializedSize () const{
    return sizeof (uint32_t)*2;
  }
  inline virtual void Serialize (TagBuffer i) const{
    i.WriteU32(m_leaf_id);
    i.WriteU32(m_interface);

  }
  inline virtual void Deserialize (TagBuffer i){
    m_leaf_id = i.ReadU32();
    m_interface = i.ReadU32 ();
  }
  inline virtual void Print (std::ostream &os) const{
    os << "leaf-id=" << (uint32_t)m_leaf_id<<"/interface-id="<<m_interface<<std::endl;
  };

  void SetLeafId (uint32_t id){
    m_leaf_id = id;
  }

  [[nodiscard]] uint32_t GetLeafId () const{
    return m_leaf_id;
  }
  void SetInterfaceId(uint32_t id){

    m_interface = id;
  }
  [[nodiscard]] uint32_t GetInterfaceId() const{
    return m_interface;
  }

private:
  uint32_t m_leaf_id;
  uint32_t m_interface;
};






class Ipv4RlRouting : public Ipv4RoutingProtocol {


public:
  Ipv4RlRouting ();
  ~Ipv4RlRouting ();

  static TypeId GetTypeId (void);

  void AddRoute (Ipv4Address network, Ipv4Mask networkMask, uint32_t port);

  /* Inherit From Ipv4RoutingProtocol */
  virtual Ptr<Ipv4Route> RouteOutput (Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr);
  virtual bool RouteInput (Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev,
                           UnicastForwardCallback ucb, MulticastForwardCallback mcb,
                           LocalDeliverCallback lcb, ErrorCallback ecb);
  virtual void NotifyInterfaceUp (uint32_t interface);
  virtual void NotifyInterfaceDown (uint32_t interface);
  virtual void NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address);
  virtual void NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address);
  virtual void SetIpv4 (Ptr<Ipv4> ipv4);
  virtual void PrintRoutingTable (Ptr<OutputStreamWrapper> stream, Time::Unit unit = Time::S) const;

  virtual void DoDispose (void);

  void init_routes(const Ptr<Ipv4>& ipv4, const Ipv4GlobalRouting::HostRoutes&, const Ipv4GlobalRouting::NetworkRoutes&, uint32_t );

  bool SetDistribution(const std::vector<float>& dist);
  inline std::vector<double> getProbs(){
    return m_distribution.probabilities();
  }
  inline void resetStats(){
    stats[0] = 0;
    stats[1] = 0;
  }
  inline std::vector<uint64_t>& getStats(){
    return stats;
  }
private:
  uint32_t m_d;
  Ptr<Ipv4> m_ipv4;
  Ipv4GlobalRouting::HostRoutes m_hostRoutes;             //!< Routes to hosts
  Ipv4GlobalRouting::NetworkRoutes m_networkRoutes;       //!< Routes to networks

  std::discrete_distribution<int> m_distribution;
  std::mt19937 m_generator;
  std::vector<uint64_t> stats;


  std::vector<Ptr<Ipv4Route>> Lookup (Ipv4Address dest, const Ptr<NetDevice>& oif = nullptr);
};

}

#endif //NS3_AARLIPV4ROUTING_H
