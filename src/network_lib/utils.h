#pragma once

#include <string>
#include <map>
#include <memory>

namespace network
{
  using Node = uint64_t; // todo: make sure this is identical with size_t
  using Variables = std::map<std::string, std::string>;

  namespace utils
  {
    std::shared_ptr<Variables> join(const Variables& v1, const Variables& v2);
  }
}
