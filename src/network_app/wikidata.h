#pragma once

#include "network_lib.h"
#include <boost/filesystem.hpp>

namespace babelscript_console
{
  namespace fs = boost::filesystem;
  
  class Wikidata
  {
  public:
    Wikidata(network::Network_lib* n);
    ~Wikidata();

    void import_json(fs::path file_name);

  private:
    void process_command(const std::wstring& line, const size_t thread_index);

    class Impl;
    Impl* const _pImpl; // must stay at top of members list because of initialization order

    network::Network_lib* _n{ nullptr };
    size_t _running{ 0 };
  };
}
