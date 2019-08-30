#pragma once

#include <filesystem>
#include <string>
#include <iosfwd>

namespace babelscript
{
  namespace console
  {
#if __cplusplus > 201402L
  namespace fs = std::filesystem;
#else
  namespace fs = std::experimental::filesystem;
#endif

    class ReadAsync
    {
    public:
      ReadAsync(fs::path file_name, size_t sufficient_size=2);
      ~ReadAsync();
      bool get_line(std::wstring& line, std::streamoff& streampos);
      std::string error_text();

    private:
      class Impl;
      Impl* const _pImpl; // must stay at top of members list because of initialization order
    };
  }
}
