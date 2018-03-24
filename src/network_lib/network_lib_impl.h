#pragma once

#include <string>
#include <cstdint>
#include <boost/bimap.hpp>
#include "network_lib.h"
#include "network.h"

namespace network
{
  class Network_lib::Impl : Network<Node>
	{
		friend class Network_lib;

	public:
    Impl()
      : _predefinedNode({ create(), create(), create(), create(), create(), create()})
    {}

	private:
		struct PredefinedNode
		{
      Node RelationType;
      Node Rule;
      Node And;
      Node IsA;
      Node Unequal;
      Node Contradiction;
    } _predefinedNode;

    std::map<std::string, boost::bimap<Node, std::wstring>> _names;
    using Names = boost::bimap<Node, std::wstring>::value_type;
	};
}
