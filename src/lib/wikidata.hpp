#pragma once

#include "utils.hpp"
#include "zelph.hpp"

#include <filesystem>
#include <iosfwd>

namespace zelph
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
            Wikidata(network::Zelph* n, fs::path file_name);
            ~Wikidata();

            void import_all();
            void generate_index();
            void traverse(int max_depth, std::wstring start_entry = L"");
            void process_node(network::Node node, std::string lang);

        private:
            void process_entry(const std::wstring& line, const bool log = true, const size_t thread_index = 0);
            void process_name(std::wstring wikidata_name);
            void index_entry(const std::wstring& line, const std::streamoff streampos);

            class Impl;
            Impl* const _pImpl; // must stay at top of members list because of initialization order

            size_t _running{0};
        };
    }
}
