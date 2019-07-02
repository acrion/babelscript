#pragma once

#include "babelscript.hpp"
#include <babelscript_export.h>

namespace babelscript
{
  namespace network
  {
    class BABELSCRIPT_EXPORT StopWatch
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

