#include "stopwatch.hpp"
#include <chrono>

using namespace babelscript::network;
namespace chrono = std::chrono;

class StopWatch::CImpl
{
public:
  chrono::high_resolution_clock::time_point _start;
  chrono::high_resolution_clock::time_point _stop;
};

StopWatch::StopWatch()
  : _pImpl(new CImpl)
{
}

StopWatch::StopWatch(const StopWatch& stopWatch)
  : _pImpl(new CImpl)
{
  *_pImpl = *stopWatch._pImpl; // memberwise copy of CImpl members (which include no pointers)
}

StopWatch& StopWatch::operator=(const StopWatch& stopWatch)
{
  *_pImpl = *stopWatch._pImpl; // memberwise copy of CImpl members (which include no pointers)
  return *this;
}

StopWatch::~StopWatch()
{
  delete _pImpl;
}

bool StopWatch::is_running()
{
  return _pImpl->_stop == chrono::time_point<chrono::high_resolution_clock>::min();
}

void StopWatch::start()
{
  _pImpl->_start = chrono::high_resolution_clock::now();
  _pImpl->_stop = chrono::time_point<chrono::high_resolution_clock>::min();
}

void StopWatch::stop()
{
  _pImpl->_stop = std::chrono::high_resolution_clock::now();
}

uint64_t StopWatch::duration()
{
  auto currentTime = is_running() ? chrono::high_resolution_clock::now() : _pImpl->_stop;
  return chrono::duration_cast<chrono::milliseconds>(currentTime - _pImpl->_start).count();
}
