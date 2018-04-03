#include "read_async.h"
#include <mutex>
#include <thread>
#include <fstream>
#include <queue>

using namespace babelscript_console;

class ReadAsync::Impl
{
public:
  Impl(fs::path file_name, size_t sufficient_size)
    : _sufficient_size(sufficient_size)
    , _t(std::thread(&Impl::read_thread, this, file_name))
  {
  }

  void read_thread(fs::path file_name);
  void put_line(const std::wstring& line);

  size_t _sufficient_size{ 10 };
  std::thread _t;
  std::queue<std::wstring> _lines;
  std::atomic<bool> _EOF{ false };
  std::string _error_text;
  std::mutex _mtx, _mtx2;
  std::condition_variable _cv, _cv2;
};

ReadAsync::ReadAsync(fs::path file_name, size_t sufficient_size)
  : _pImpl(new Impl(file_name, sufficient_size))
{

}

ReadAsync::~ReadAsync()
{
  delete _pImpl;
}


std::string ReadAsync::error_text()
{
  std::lock_guard<std::mutex> lck(_pImpl->_mtx);
  return _pImpl->_error_text;
}

bool ReadAsync::get_line(std::wstring& line)
{
  std::unique_lock<std::mutex> lock(_pImpl->_mtx);
  _pImpl->_cv.wait(lock, [&]{ return _pImpl->_lines.size() > 0; });

  if (_pImpl->_EOF)
  {
    return false;
  }
  line = _pImpl->_lines.front();
  std::lock_guard<std::mutex> lck(_pImpl->_mtx2);
  _pImpl->_lines.pop();
  _pImpl->_cv2.notify_one();
  return true;
}

void ReadAsync::Impl::put_line(const std::wstring& line)
{
  bool done = false;
  
  do
  {
    {
      std::lock_guard<std::mutex> lck(_mtx);

      if (_lines.size() < _sufficient_size)
      {
        _lines.push(line);
        done = true;
      }
    }
    
    if (!done)
    {
      std::unique_lock<std::mutex> lock(_mtx2);
      _cv2.wait(lock, [&] { return _lines.size() < _sufficient_size; });
    }
  }
  while (!done);

  _cv.notify_one();
}

void ReadAsync::Impl::read_thread(fs::path file_name)
{
  std::wifstream stream(file_name.string());

  if (stream.fail())
  {
    std::lock_guard<std::mutex> lck(_mtx);
    _error_text = std::string("Could not open file '") + file_name.string() + "'";
    _EOF = true;
    return;
  }

  for (std::wstring line; std::getline(stream, line); )
  {
    put_line(line);
  }
}
