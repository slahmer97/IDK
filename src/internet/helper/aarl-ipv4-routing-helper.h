//
// Created by slahmer on 3/2/22.
//

#ifndef NS3_AARLIPV4ROUTINGHELPER_H
#define NS3_AARLIPV4ROUTINGHELPER_H

#include "ns3/aarl-ipv4-routing.h"
#include "ns3/ipv4-routing-helper.h"

namespace ns3 {

class Ipv4RlRoutingHelper : public Ipv4RoutingHelper
{
public:
  Ipv4RlRoutingHelper ();
  Ipv4RlRoutingHelper (const Ipv4RlRoutingHelper&);

  [[nodiscard]] Ipv4RlRoutingHelper *Copy () const override;

  [[nodiscard]] Ptr<Ipv4RoutingProtocol> Create (Ptr<Node> node) const override;

  [[nodiscard]] static Ptr<Ipv4RlRouting> GetRLRouting (Ptr<Ipv4> ipv4) ;
};

}

#endif //NS3_AARLIPV4ROUTINGHELPER_H
