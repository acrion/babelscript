#pragma once

#include <string>
#include <cstdint>
#include <map>
#include <mutex>
#include "network_lib.h"
#include "network.h"

namespace network
{
  class NETWORK_LIB_EXPORT Network_lib::Impl : public Network
  {
    friend class Network_lib;

  private:
    // cannot use boost:bitmap because we have to support nodes having the same name (within one language). In this case, _node_of_name has no entries for all of these names.
    std::map<std::string, std::map<Node, std::wstring>> _name_of_node; // key is language identifier
    std::map<std::string, std::map<std::wstring, Node>> _node_of_name; // key is language identifier

    std::mutex _mtx_name_of_node;
    std::mutex _mtx_node_of_name;
  };
}
