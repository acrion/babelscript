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
      Wikidata(network::Babelscript* n, fs::path file_name);
      ~Wikidata();

      void import_all();
      void generate_index();
      void traverse(std::wstring start_entry, int max_depth);

    private:
      void process_entry(const std::wstring& line, const size_t streampos, const size_t thread_index=0);
      void index_entry(const std::wstring& line, const size_t streampos);

      class Impl;
      Impl* const _pImpl; // must stay at top of members list because of initialization order

      size_t _running{ 0 };
    };
  }
}
