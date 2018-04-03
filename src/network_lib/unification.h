#pragma once

#include "network_lib.h"
#include "utils.h"
#include <memory>
#include <map>
#include <unordered_set>

namespace network
{
  class Unification
  {
  public:
    Unification(Network_lib* n, Node condition, Node parent, std::shared_ptr<Variables> variables, std::shared_ptr<Variables> unequals);
    std::shared_ptr<Variables> Next();
    std::shared_ptr<Variables> Unequals();

  protected:
    Network_lib* const _n;
    Node _parent;
    std::shared_ptr<Variables> _variables;
    std::shared_ptr<Variables> _unequals;
    std::unordered_set<Node> _relation_list;
    Node _relation_variable{ 0 };
    std::unordered_set<Node>::iterator _fact_index{ decltype(_facts_of_current_relation->second)::iterator() };
    std::unordered_set<Node>::iterator _relation_index;
    Node _subject;
    std::unordered_set<Node> _objects;

  private:
    bool increment_fact_index();
    std::map<Node, std::unordered_set<Node>>::iterator _facts_of_current_relation;
    bool _initialized{ false };
  };
}
