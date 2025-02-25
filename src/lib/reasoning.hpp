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

#include "babelscript.hpp"
#include "stopwatch.hpp"

#include <babelscript_export.h>

#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace babelscript
{
    namespace network
    {
        struct RulePos
        {
            Node                               node;
            std::unordered_set<Node>::iterator end;
            std::unordered_set<Node>::iterator index;
            std::shared_ptr<Variables>         variables{std::make_shared<Variables>()};
            std::shared_ptr<Variables>         unequals{std::make_shared<Variables>()};
        };

        class BABELSCRIPT_EXPORT Reasoning : public Babelscript
        {
        public:
            explicit Reasoning(std::function<void(std::wstring, bool)>);
            void run(bool print_deductions);
            void apply_rule(const network::Node& rule, network::Node condition, size_t thread_index);

        private:
            void evaluate(RulePos rule);
            bool contradicts(const Variables& variables, const Variables& unequals);
            void deduce(const Variables& variables, Node parent);

            bool                     _done{false};
            Node                     _current_condition{0};
            std::unordered_set<Node> _deductions;
            std::vector<RulePos>     _next;
            size_t                   _running{0};
            bool                     _print_deductions{true};
            bool                     _contradiction{false};
            StopWatch                _stop_watch;
            size_t                   _skipped{0};
        };
    }
}
