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

#include "utils.hpp"

#include <codecvt>
#include <locale>
#include <stdexcept>

namespace zelph
{
    namespace network
    {
        namespace utils
        {
            std::shared_ptr<Variables> join(const Variables& v1, const Variables& v2)
            {
                std::shared_ptr<Variables> result = std::make_shared<Variables>(v1);

                for (auto& var : v2)
                {
                    auto it = result->find(var.first);

                    if (it != result->end())
                    {
                        if (it->second != var.second)
                        {
                            throw std::runtime_error("Variable sets to be merged do conflict");
                        }
                    }
                    else
                    {
                        (*result)[var.first] = var.second;
                    }
                }

                return result;
            }

            std::string str(std::wstring str)
            {
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                return converter.to_bytes(str);
            }

            std::wstring wstr(std::string str)
            {
                return std::wstring(str.begin(), str.end());
            }

            std::wstring mark_identifier(const std::wstring& str)
            {
                if (str.empty()
                    || std::isupper(static_cast<unsigned char>(str.back()))
                    || str.front() == L'('
                    || str.back() == L')'
                    || str.front() == L'!')
                {
                    return str;
                }
                else
                {
                    return L"«" + str + L"»";
                }
            }
        }
    }
}
