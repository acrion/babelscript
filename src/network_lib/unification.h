#pragma once

#include "answer.h"
#include <string>
#include <map>
#include <memory>

namespace network
{
  class Unification
  {
  public:
    Unification(Node condition, std::shared_ptr<std::map<std::string, std::string>> variables, std::shared_ptr<std::map<std::string, std::string>> unequals);
  };
}
