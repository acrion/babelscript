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
    std::shared_ptr<Variables> _variables;
    std::shared_ptr<Variables> _unequals;
    std::set<Node> _relation_list;
    Node _relation_variable{ 0 };
    std::set<Node>::iterator _fact_index;
    std::set<Node>::iterator _relation_index;
  };
}
