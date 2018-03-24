#pragma once

#include <string>
#include <cstdint>
#include <boost/bimap.hpp>
#include "network_lib.h"
#include "network.h"

namespace network
{
  class Network_lib::Impl : Network
	{
		friend class Network_lib;

	public:
    Impl()
      : _p({ create(), create(), create(), create(), create(), create()})
    {}

		struct PredefinedNode
		{
      Node RelationTypeCategory;
      Node Rule;
      Node And;
      Node IsA;
      Node Unequal;
      Node Contradiction;
    } _p;

  private:
    std::map<std::string, boost::bimap<Node, std::wstring>> _names;
    using Names = boost::bimap<Node, std::wstring>::value_type;

  protected:
    std::wstring get_name(Node node, decltype(_names)::iterator namesInLang);
  };
}
