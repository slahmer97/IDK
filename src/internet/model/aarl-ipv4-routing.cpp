//
// Created by slahmer on 3/1/22.
//

#include <iomanip>
#include <random>
#include "aarl-ipv4-routing.h"
#include "ns3/core-module.h"
#include "ipv4-list-routing.h"
#include "ipv4-routing-table-entry.h"
#include "ns3/point-to-point-module.h"
NS_LOG_COMPONENT_DEFINE ("Ipv4RlRouting");


ns3::Ipv4RlRouting::Ipv4RlRouting ()
{
  //std::cout<<"==========================================Ipv4RlRouting()"<<this->m_ipv4<<std::endl;
  m_generator.seed(std::random_device()());
  m_distribution = {};
  stats.push_back (0);
  stats.push_back (0);

}


ns3::Ipv4RlRouting::~Ipv4RlRouting ()
{
}


ns3::TypeId
ns3::Ipv4RlRouting::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Ipv4RlRouting")
                          .SetParent<Object> ()
                          .SetGroupName ("RlRouting")
                          .AddConstructor<Ipv4RlRouting> ()
                          .AddAttribute ("d", "Sample d random outputs queue",
                                         UintegerValue (2),
                                         MakeUintegerAccessor (&Ipv4RlRouting::m_d),
                                         MakeUintegerChecker<uint32_t> ())
      ;

  return tid;
}


void
ns3::Ipv4RlRouting::AddRoute (ns3::Ipv4Address network, ns3::Ipv4Mask networkMask, uint32_t port)
{
  std::cout<<"==========================================AddRoute"<<std::endl;

}



ns3::Ptr<ns3::Ipv4Route>
ns3::Ipv4RlRouting::RouteOutput (ns3::Ptr<ns3::Packet> p, const ns3::Ipv4Header &header,
                                 ns3::Ptr<ns3::NetDevice> oif, ns3::Socket::SocketErrno &sockerr)
{
  std::cout<<"==========================================RouteOutput(NotEmplementedYet)"<<this->m_ipv4<<std::endl;

  return {};
}


bool
ns3::Ipv4RlRouting::RouteInput (ns3::Ptr<const ns3::Packet> p, const ns3::Ipv4Header &header,
                                ns3::Ptr<const ns3::NetDevice> idev,
                                ns3::Ipv4RoutingProtocol::UnicastForwardCallback ucb,
                                ns3::Ipv4RoutingProtocol::MulticastForwardCallback mcb,
                                ns3::Ipv4RoutingProtocol::LocalDeliverCallback lcb,
                                ns3::Ipv4RoutingProtocol::ErrorCallback ecb)
{

  //std::cout<<"==========================================RouteInput()"<<this->m_ipv4<<std::endl;
  NS_LOG_FUNCTION (this << p << header << header.GetSource () << header.GetDestination () << idev << &lcb << &ecb);
  // Check if input device supports IP
  NS_ASSERT (m_ipv4->GetInterfaceForDevice (idev) >= 0);
  uint32_t iif = m_ipv4->GetInterfaceForDevice (idev);
  Ptr<Packet> packet = ConstCast<Packet> (p);

  Ipv4Address destAddress = header.GetDestination();

  if (destAddress.IsMulticast() || destAddress.IsBroadcast()) {
      NS_LOG_ERROR (this << " Drill routing only supports unicast");
      ecb (packet, header, Socket::ERROR_NOROUTETOHOST);
      return false;
  }

  if (!m_ipv4->IsForwarding (iif)) {
      NS_LOG_ERROR (this << " Forwarding disabled for this interface");
      ecb (packet, header, Socket::ERROR_NOROUTETOHOST);
      return false;
    }

  if (m_ipv4->IsDestinationAddress (header.GetDestination (), iif))
    {
      if (!lcb.IsNull ())
        {
          NS_LOG_LOGIC ("Local delivery to " << header.GetDestination ());
          lcb (p, header, iif);
          return true;
        }
      else
        {
          return false;
        }
    }


  NS_LOG_LOGIC ("Unicast destination- looking up global route");
  std::vector<Ptr<Ipv4Route>> candidates = Lookup(header.GetDestination ());
  //std::cout<<"Condidates: "<<candidates.size()<<std::endl;
  //auto node_id = this->m_ipv4->GetNetDevice (0)->GetNode()->GetId();
  if (candidates.size() == 1)
    {
     // std::cout<<"111------------("<<node_id<<")"<<std::endl;
      NS_LOG_LOGIC ("Found unicast destination- calling unicast callback");
      ucb (candidates[0], p, header);
      return true;
    }
  else if (candidates.size()>1)
  {
      //std::cout<<"candidates="<<candidates.size()<<",pobabilities="<<m_distribution.probabilities().size()<<"("<<node_id<<")"<<std::endl;

      NS_ASSERT (candidates.size() == m_distribution.probabilities().size());

      auto index = m_distribution(m_generator);
      stats[index]++;
      //MyTag tmp;
      auto outputRoute = candidates[index];
      //tmp.SetInterfaceId (outputRoute->GetOutputDevice()->GetIfIndex());
      //tmp.SetLeafId(outputRoute->GetOutputDevice()->GetNode()->GetId());
      //p->AddPacketTag(tmp);
      ucb (outputRoute, p, header);
      //MyTag tagCopy;
      //p->PeekPacketTag (tagCopy);
      //p->PrintPacketTags (std::cout);
      //std::cout<<std::endl;
      /*
      auto tmp1 = DynamicCast<PointToPointChannel>(DynamicCast<PointToPointNetDevice>(candidates[0]->GetOutputDevice())->GetChannel());
      auto tmp2 = DynamicCast<PointToPointChannel>(DynamicCast<PointToPointNetDevice>(candidates[1]->GetOutputDevice())->GetChannel());
      std::cout<<"Node id: "<<candidates[0]->GetOutputDevice()->GetNode()->GetId()<<std::endl;
      std::cout<<"\tid1: "<<tmp1->GetDevice (1)->GetNode()->GetId()<<std::endl;
      std::cout<<"\tid2: "<<tmp2->GetDevice (1)->GetNode()->GetId()<<std::endl;
      */
  }

  else
  {
        // Check other missing case in the above condition
        NS_LOG_LOGIC ("Did not find unicast destination- returning false");
        return false; // Let other routing protocols try to handle this
  }
  return false;
}


void
ns3::Ipv4RlRouting::NotifyInterfaceUp (uint32_t interface)
{
  //std::cout<<"==========================================NotifyInterfaceUp()"<<this->m_ipv4<<std::endl;

}


bool ns3::Ipv4RlRouting::SetDistribution(const std::vector<float>& dist)
{
  //std::cout<<"SetDistribution("<<dist[0]<<","<<dist[1]<<")\n";
  //std::cout<<"size("<<dist.size()<<")\n";

  m_distribution = std::discrete_distribution<int>(dist.begin(), dist.end());
  auto tmp = m_distribution.probabilities();

  //std::cout<<"NewDist("<<tmp[0]<<","<<tmp[1]<<")\n";
  //std::cout<<"NewSize("<<tmp.size()<<")\n";

  return true;
}

void
ns3::Ipv4RlRouting::NotifyInterfaceDown (uint32_t interface)
{
  //std::cout<<"==========================================NotifyInterfaceDown()"<<this->m_ipv4<<std::endl;

}


void
ns3::Ipv4RlRouting::NotifyAddAddress (uint32_t interface, ns3::Ipv4InterfaceAddress address)
{
  //std::cout<<"==========================================NotifyAddAddress()"<<this->m_ipv4<<std::endl;

}


void
ns3::Ipv4RlRouting::NotifyRemoveAddress (uint32_t interface, ns3::Ipv4InterfaceAddress address)
{
  //std::cout<<"==========================================NotifyRemoveAddress()"<<this->m_ipv4<<std::endl;
}


void
ns3::Ipv4RlRouting::SetIpv4 (ns3::Ptr<ns3::Ipv4> ipv4)
{
 //
}


void
ns3::Ipv4RlRouting::PrintRoutingTable (ns3::Ptr<ns3::OutputStreamWrapper> stream,
                                       ns3::Time::Unit unit) const
{
  //

}


void
ns3::Ipv4RlRouting::DoDispose (void)
{
  Object::DoDispose ();
}

void
ns3::Ipv4RlRouting::init_routes (const Ptr<Ipv4>& ipv4,const Ipv4GlobalRouting::HostRoutes& host_routes, const Ipv4GlobalRouting::NetworkRoutes& network_routes, uint32_t action_space)
{
  m_ipv4 = ipv4;
  for (auto host_route : host_routes)
  {

    auto *route =  new Ipv4RoutingTableEntry();
    *route = Ipv4RoutingTableEntry::CreateHostRouteTo(host_route->GetDest(),
                                                       host_route->GetGateway(),
                                                       host_route->GetInterface());
    m_hostRoutes.push_back (route);
  }

  for (auto network_route : network_routes)
    {

      auto *route =  new Ipv4RoutingTableEntry();
      *route = Ipv4RoutingTableEntry::CreateNetworkRouteTo(network_route->GetDestNetwork(),
                                                         network_route->GetDestNetworkMask(),
                                                         network_route->GetGateway(), network_route->GetInterface());
      m_networkRoutes.push_back (route);
    }
  std::cout<<"Init_routes m_hostRoutes: "<<m_hostRoutes.size()<<" -- m_networkRoutes: "<<m_networkRoutes.size()<<std::endl;
}


std::vector<ns3::Ptr<ns3::Ipv4Route>>
ns3::Ipv4RlRouting::Lookup (ns3::Ipv4Address dest, const ns3::Ptr<ns3::NetDevice>& oif)
{
  NS_LOG_FUNCTION (this << dest << oif);
  NS_LOG_LOGIC ("Looking for route for destination " << dest);
  Ptr<Ipv4Route> rtentry = 0;
  std::vector<Ptr<Ipv4Route>> ret;
  // store all available routes that bring packets to their destination
  typedef std::vector<Ipv4RoutingTableEntry*> RouteVec_t;
  RouteVec_t allRoutes;

  NS_LOG_LOGIC ("Number of m_hostRoutes = " << m_hostRoutes.size ());
  for (auto i = m_hostRoutes.begin ();
       i != m_hostRoutes.end ();
       i++)
    {
      NS_ASSERT ((*i)->IsHost ());
      if ((*i)->GetDest () == dest)
        {
          if (oif != 0)
            {
              if (oif != m_ipv4->GetNetDevice ((*i)->GetInterface ()))
                {
                  NS_LOG_LOGIC ("Not on requested interface, skipping");
                  continue;
                }
            }
          allRoutes.push_back (*i);
          NS_LOG_LOGIC (allRoutes.size () << "Found global host route" << *i);
        }
    }
  if (allRoutes.size () == 0) // if no host route is found
    {
      NS_LOG_LOGIC ("Number of m_networkRoutes" << m_networkRoutes.size ());
      for (auto j = m_networkRoutes.begin ();
           j != m_networkRoutes.end ();
           j++)
        {
          Ipv4Mask mask = (*j)->GetDestNetworkMask ();
          Ipv4Address entry = (*j)->GetDestNetwork ();
          if (mask.IsMatch (dest, entry))
            {
              if (oif != 0)
                {
                  if (oif != m_ipv4->GetNetDevice ((*j)->GetInterface ()))
                    {
                      NS_LOG_LOGIC ("Not on requested interface, skipping");
                      continue;
                    }
                }
              allRoutes.push_back (*j);
              NS_LOG_LOGIC (allRoutes.size () << "Found global network route" << *j);
            }
        }
    }

    for (const auto &item : allRoutes){
        Ipv4RoutingTableEntry* route = item;
        // create a Ipv4Route object from the selected routing table entry
        rtentry = Create<Ipv4Route> ();
        rtentry->SetDestination (route->GetDest ());
        /// \todo handle multi-address case
        rtentry->SetSource (m_ipv4->GetAddress (route->GetInterface (), 0).GetLocal ());
        rtentry->SetGateway (route->GetGateway ());
        uint32_t interfaceIdx = route->GetInterface ();
        rtentry->SetOutputDevice (m_ipv4->GetNetDevice (interfaceIdx));
        ret.push_back (rtentry);

    }



    return ret;
}
