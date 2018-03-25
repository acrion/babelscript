#pragma once

#include "answer.h"
#include <memory>

namespace network
{
  class Unification
  {
  public:
    Unification(Node condition, std::shared_ptr<Variables> variables, std::shared_ptr<Variables> unequals);
    std::shared_ptr<Variables> Start();
    std::shared_ptr<Variables> Next();
    std::shared_ptr<Variables> Unequals();
  };
}
