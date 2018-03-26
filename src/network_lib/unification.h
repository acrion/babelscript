#pragma once

#include "network_lib.h"
#include <memory>

namespace network
{
  class Unification
  {
  public:
    Unification(Network_lib* n, Node condition, std::shared_ptr<Variables> variables, std::shared_ptr<Variables> unequals);
    std::shared_ptr<Variables> Start();
    std::shared_ptr<Variables> Next();
    std::shared_ptr<Variables> Unequals();

  protected:
    Network_lib* const _n;
  };
}
