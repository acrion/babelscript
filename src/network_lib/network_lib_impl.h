#pragma once

#include <string>
#include <cstdint>
#include <boost/bimap.hpp>
#include "network_lib.h"
#include "network.h"

namespace network
{
	class Network_lib::Impl
	{
		friend class Network_lib;

	public:

	private:
		struct PredefinedNode
		{
      static constexpr uint64_t RelationType{ 1 };
      static constexpr uint64_t Rule{ 2 };
      static constexpr uint64_t And{ 3 };
      static constexpr uint64_t IsA{ 4 };
      static constexpr uint64_t Unequal{ 5 };
      static constexpr uint64_t Contradiction{ 6 };
		};

    uint64_t _lastNode{ PredefinedNode::Contradiction };
    Network<uint64_t> _network;
    std::map<std::string, boost::bimap<uint64_t, std::wstring>> _names;
    using Names = boost::bimap<uint64_t, std::wstring>::value_type;
	};
}
