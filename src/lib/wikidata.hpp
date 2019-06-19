#pragma once

#include "babelscript.hpp"
#include <filesystem>

namespace babelscript
{
  namespace console
  {
#if __cplusplus > 201402L
  namespace fs = std::filesystem;
#else
  namespace fs = std::experimental::filesystem;
#endif

    class Wikidata
    {
    public:
      Wikidata(network::Babelscript* n);
      ~Wikidata();

      void import_json(fs::path file_name);

    private:
      void process_command(const std::wstring& line, const size_t thread_index);

      class Impl;
      Impl* const _pImpl; // must stay at top of members list because of initialization order

      network::Babelscript* _n{ nullptr };
      size_t _running{ 0 };
    };
  }
}

