#include "stopwatch.h"
#include <chrono>

using namespace network;
namespace chrono = std::chrono;

class StopWatch::CImpl
{
public:
  chrono::high_resolution_clock::time_point m_start;
  chrono::high_resolution_clock::time_point m_stop;
};

StopWatch::StopWatch()
  : _pImpl(new CImpl)
{
}

StopWatch::StopWatch(const StopWatch& stopWatch)
  : _pImpl(new CImpl)
{
  *_pImpl = *stopWatch._pImpl;
}

StopWatch::~StopWatch()
{
  delete _pImpl;
}

bool StopWatch::is_running()
{
  return _pImpl->m_stop == chrono::time_point<chrono::steady_clock>::min();
}

void StopWatch::start()
{
  _pImpl->m_start = chrono::high_resolution_clock::now();
  _pImpl->m_stop = chrono::time_point<chrono::steady_clock>::min();
}

void StopWatch::stop()
{
  _pImpl->m_stop = std::chrono::high_resolution_clock::now();
}

uint64_t StopWatch::duration()
{
  auto currentTime = is_running() ? chrono::high_resolution_clock::now() : _pImpl->m_stop;
  return chrono::duration_cast<chrono::milliseconds>(currentTime - _pImpl->m_start).count();
}
