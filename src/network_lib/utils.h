#pragma once

#include <string>
#include <map>
#include <memory>

namespace network
{
  using Node = uint64_t; // todo: make sure this is identical with size_t
  using Variables = std::map<Node, Node>;

  namespace utils
  {
    std::shared_ptr<Variables> join(const Variables& v1, const Variables& v2);
    std::string str(std::wstring str);
    std::wstring wstr(std::string str);

    template<typename T, typename U, typename V>
    static T get(U container, V key, T return_if_not_found)
    {
      auto it = container.find(key);
      return it == container.end() ? return_if_not_found : it->second;
    }
  }
}
