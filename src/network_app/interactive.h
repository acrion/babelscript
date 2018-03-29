#pragma once

#include <string>
#include <map>

namespace babelscript_console
{
  class Interactive
  {
  public:
    Interactive();
    ~Interactive();
    void process(std::wstring line);
    void run();
    static std::string get_version();
    static bool is_var(std::wstring token);

  private:
    class Impl;
    Impl* const _pImpl; // must stay at top of members list because of initialization order
  };
}
