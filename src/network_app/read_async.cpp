#include "read_async.h"
#include <mutex>
#include <thread>
#include <fstream>
#include <queue>

using namespace babelscript_console;

class ReadAsync::Impl
{
public:
  Impl(fs::path file_name)
    : _t(std::thread(&Impl::read_thread, this, file_name))
  {
  }

  void read_thread(fs::path file_name);
  void put_line(const std::wstring& line);

  std::queue<std::wstring> _lines;
  std::atomic<bool> _EOF{ false };
  std::string _error_text;
  std::mutex _mtx;
  std::condition_variable _cv;
  bool _processed{ false };
  std::thread _t;
};

ReadAsync::ReadAsync(fs::path file_name)
  : _pImpl(new Impl(file_name))
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
  {
    std::lock_guard<std::mutex> lck(_pImpl->_mtx);
    if (_pImpl->_lines.size() > 0)
    {
      line = _pImpl->_lines.front();
      _pImpl->_processed = false;
      _pImpl->_lines.pop();
      return true;
    }
  }

  if (_pImpl->_EOF)
  {
    _pImpl->_processed = false;
    return false;
  }

  std::unique_lock<std::mutex> lock(_pImpl->_mtx);
  _pImpl->_cv.wait(lock, [&]{ return _pImpl->_processed; });

  if (_pImpl->_EOF)
  {
    _pImpl->_processed = false;
    return false;
  }
  line = _pImpl->_lines.front();
  _pImpl->_processed = false;
  _pImpl->_lines.pop();
  return true;
}

void ReadAsync::Impl::put_line(const std::wstring& line)
{
  {
    std::lock_guard<std::mutex> lck(_mtx);
    _lines.push(line);
    _processed = true;
  }
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
