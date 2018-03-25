#pragma once

#include <string>
#include <cstdint>
#include <boost/bimap.hpp>
#include "network_lib.h"
#include "network.h"

namespace network
{
  class NETWORK_LIB_EXPORT Network_lib::Impl : public Network
  {
    friend class Network_lib;

  private:
    std::map<std::string, boost::bimap<Node, std::wstring>> _names;
    using Names = boost::bimap<Node, std::wstring>::value_type;

  protected:
    std::wstring get_name(Node node, decltype(_names)::iterator namesInLang);
  };
}
