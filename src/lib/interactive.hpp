#pragma once

#include <string>
#include <map>
#include <babelscript_export.h>

namespace babelscript
{
  namespace console
  {
    class BABELSCRIPT_EXPORT Interactive
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
