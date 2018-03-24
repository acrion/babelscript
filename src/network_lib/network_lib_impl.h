#pragma once

#include <string>
#include <cstdint>
#include <boost/bimap.hpp>
#include "network_lib.h"
#include "network.h"

namespace network
{
  class Network_lib::Impl : Network<uint64_t>
	{
		friend class Network_lib;

	public:
    Impl()
    {
      _predefinedNode.RelationType  = create();
      _predefinedNode.Rule          = create();
      _predefinedNode.And           = create();
      _predefinedNode.IsA           = create();
      _predefinedNode.Unequal       = create();
      _predefinedNode.Contradiction = create();
    }

	private:
		struct PredefinedNode
		{
      uint64_t RelationType;
      uint64_t Rule;
      uint64_t And;
      uint64_t IsA;
      uint64_t Unequal;
      uint64_t Contradiction;
    } _predefinedNode;

    std::map<std::string, boost::bimap<uint64_t, std::wstring>> _names;
    using Names = boost::bimap<uint64_t, std::wstring>::value_type;
	};
}
