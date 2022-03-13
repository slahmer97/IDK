//
// Created by slahmer on 3/2/22.
//

#include "aarl-ipv4-routing-helper.h"
ns3::Ipv4RlRoutingHelper::Ipv4RlRoutingHelper ()
{
}
ns3::Ipv4RlRoutingHelper::Ipv4RlRoutingHelper (const ns3::Ipv4RlRoutingHelper &)
{
}
ns3::Ipv4RlRoutingHelper *
ns3::Ipv4RlRoutingHelper::Copy () const
{
  return new Ipv4RlRoutingHelper (*this);;
}
ns3::Ptr<ns3::Ipv4RoutingProtocol>
ns3::Ipv4RlRoutingHelper::Create (ns3::Ptr<ns3::Node> node) const
{
  Ptr<Ipv4RlRouting> rlrouting = CreateObject<Ipv4RlRouting> ();
  return rlrouting;
}


ns3::Ptr<ns3::Ipv4RlRouting>
ns3::Ipv4RlRoutingHelper::GetRLRouting (ns3::Ptr<ns3::Ipv4> ipv4)
{
  Ptr<Ipv4RoutingProtocol> ipv4rp = ipv4->GetRoutingProtocol ();
  if (DynamicCast<Ipv4RlRouting> (ipv4rp))
    {
      return DynamicCast<Ipv4RlRouting> (ipv4rp);
    }
  if (DynamicCast<Ipv4ListRouting> (ipv4rp))
    {
      Ptr<Ipv4ListRouting> lrp = DynamicCast<Ipv4ListRouting> (ipv4rp);
      int16_t priority;
      for (uint32_t i = 0; i < lrp->GetNRoutingProtocols ();  i++)
        {
          Ptr<Ipv4RoutingProtocol> temp = lrp->GetRoutingProtocol (i, priority);
          if (DynamicCast<Ipv4RlRouting> (temp))
            {
              return DynamicCast<Ipv4RlRouting> (temp);
            }
        }
    }

  return nullptr;
}
