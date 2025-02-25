/*
Copyright (c) 2025 acrion innovations GmbH
Authors: Stefan Zipproth, s.zipproth@acrion.ch

This file is part of zelph, see https://github.com/acrion/zelph and https://zelph.org

zelph is offered under a commercial and under the AGPL license.
For commercial licensing, contact us at https://acrion.ch/sales. For AGPL licensing, see below.

AGPL licensing:

zelph is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

zelph is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with zelph. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "answer.hpp"

#include <babelscript_export.h>

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

namespace babelscript
{
    namespace network
    {
        class BABELSCRIPT_EXPORT Babelscript
        {
        public:
            explicit Babelscript(std::function<void(std::wstring, bool)> print);
            ~Babelscript();

            Node var();

            void                         set_lang(const std::string& lang) { _lang = lang; }
            void                         set_print(std::function<void(std::wstring, bool)> print) { _print = print; }
            void                         set_process_node(std::function<void(Node, std::string)> process_node) { _process_node = process_node; }
            std::string                  lang() { return _lang; }
            Node                         node(std::wstring name, std::string lang = "");
            bool                         has_name(Node node, std::string lang);
            std::wstring                 get_name(Node node, std::string lang = "", bool fallback = false);
            std::map<Node, std::wstring> get_nodes_in_language(std::string lang);
            Node                         get_node(std::wstring name, std::string lang = "");
            std::string                  get_name_hex(Node node, bool prepend_num = true);
            void                         set_name(Node node, std::wstring name, std::string lang = "");

            std::unordered_set<Node> get_sources(Node relationType, Node target, bool exclude_vars = false);
            Node                     parse_fact(Node rule, std::unordered_set<Node>& deducations, Node parent = 0);
            Node                     parse_relation(Node rule);
            void                     format_fact(std::wstring& result, const std::string& lang, Node fact, const Variables& variables = {}, Node parent = 0, std::shared_ptr<std::unordered_set<Node>> history = nullptr);
            std::unordered_set<Node> filter(std::unordered_set<Node> source, Node target);
            std::unordered_set<Node> filter(Node fact, Node relationType, Node target);
            std::unordered_set<Node> filter(std::unordered_set<Node> source, std::function<bool(Node nd)> f);
            Answer                   check_fact(Node source, Node relationType, const std::unordered_set<Node>& targets);
            Node                     fact(Node source, Node relationType, std::unordered_set<Node> targets, long double probability = 1);
            Node                     condition(Node op, std::unordered_set<Node> conditions);
            void                     gen_dot(Node start, std::string file_name, int max_depth);
            void                     print(std::wstring, bool);
            static const std::string get_version();

            // member list
            class Impl;
            Impl* const _pImpl; // must stay at top of members list because of initialization order

            const struct PredefinedNode
            {
                Node RelationTypeCategory;
                Node Causes;
                Node And;
                Node IsA;
                Node Unequal;
                Node Contradiction;
            } core;

        protected:
            std::string _lang{"en"};

        private:
            void add_nodes(Node current, std::unordered_set<Node>& touched, const std::unordered_set<Node>& conditions, const std::unordered_set<Node>& deductions, std::ofstream& dot, int max_depth);

            std::function<void(std::wstring, bool)> _print;
            std::function<void(Node, std::string)>  _process_node;
        };
    }
}
