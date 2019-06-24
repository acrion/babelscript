#pragma once

#include <string>
#include <map>

namespace babelscript
{
  namespace console
  {
    class Interactive
    {
    public:
      Interactive();
      ~Interactive();
      void process(std::wstring line);
      void run(bool print_deductions);
      static std::string get_version();

    private:
      class Impl;
      Impl* const _pImpl; // must stay at top of members list because of initialization order
    };
  }
}
