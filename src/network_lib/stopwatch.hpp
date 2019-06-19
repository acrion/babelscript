#pragma once

#include "network_lib.hpp"
#include "network_lib_export.h"

namespace babelscript
{
  namespace network
  {
    class NETWORK_LIB_EXPORT StopWatch
    {
    public:
      StopWatch();
      ~StopWatch();
      StopWatch(const StopWatch& stopWatch);
      bool is_running();
      void stop();
      void start();
      uint64_t duration();

    private:
      class CImpl;
      CImpl* const _pImpl{ nullptr };
    };
  }
}

