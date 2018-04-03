#pragma once

#include "network_lib.h"

namespace babelscript_console
{
  class StopWatch
  {
  public:
    StopWatch();
    ~StopWatch();
    StopWatch(const StopWatch& stopWatch);
    bool is_running();
    void stop();
    uint64_t duration();

  private:
    class CImpl;
    CImpl* const _pImpl{ nullptr };
  };
}
