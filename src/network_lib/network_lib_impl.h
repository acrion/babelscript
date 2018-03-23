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
			static constexpr int64_t RelationType{ 1 };
      static constexpr int64_t Rule{ 2 };
			static constexpr int64_t And{ 3 };
      static constexpr int64_t IsA{ 4 };
      static constexpr int64_t Unequal{ 5 };
      static constexpr int64_t Contradiction{ 6 };
		};

		int64_t _lastNode{ PredefinedNode::Contradiction };
    Network<int64_t,int64_t> _network;
		std::map<std::string, boost::bimap<int64_t, std::wstring>> _names;
		using Names = boost::bimap<int64_t, std::wstring>::value_type;
	};
}
