#pragma once
#include <babelscript_export.h>
#include <string>
#include <map>
#include <memory>
#include <vector>

namespace babelscript
{
  namespace network
  {
    using Node = uint64_t; // todo: make sure this is identical with size_t
    using Variables = std::map<Node, Node>;

    namespace utils
    {
      std::shared_ptr<Variables> join(const Variables& v1, const Variables& v2);
      std::string BABELSCRIPT_EXPORT str(std::wstring str);
      std::wstring BABELSCRIPT_EXPORT wstr(std::string str);

      template<typename T, typename U, typename V>
      static T get(U container, V key, T return_if_not_found)
      {
        auto it = container.find(key);
        return it == container.end() ? return_if_not_found : it->second;
      }

      template<typename T, typename U>
      static T get(U container, T key)
      {
        auto it = container.find(key);
        return it == container.end() ? key : it->second;
      }

      template<typename T>
      static std::basic_string<T> concatenate(std::vector<std::basic_string<T>> list, T separator)
      {
        std::basic_string<T> connected;
        for (std::basic_string<T> t : list)
        {
          if (!connected.empty()) connected += separator;
          connected += t;
        }
        return connected;
      }
    }
  }
}

