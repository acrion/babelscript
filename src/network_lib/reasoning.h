#pragma once

#include "network_lib_export.h"
#include "network_lib.h"

namespace network
{
  class NETWORK_LIB_EXPORT Reasoning : public Network_lib
  {
  public:
    void run();

  private:
    bool _done;
  };
}
