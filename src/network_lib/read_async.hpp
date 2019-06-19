#pragma once

#include <boost/filesystem.hpp>
#include <string>

namespace babelscript
{
  namespace console
  {
    namespace fs = boost::filesystem;
    
    class ReadAsync
    {
    public:
      ReadAsync(fs::path file_name, size_t sufficient_size=2);
      ~ReadAsync();
      bool get_line(std::wstring& line);
      std::string error_text();

    private:
      class Impl;
      Impl* const _pImpl; // must stay at top of members list because of initialization order
    };
  }
}

