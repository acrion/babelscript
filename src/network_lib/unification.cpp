#pragma once

#include <unification.h>
#include "network_lib_impl.h"


using namespace network;

Unification::Unification(Network_lib* n, Node condition, std::shared_ptr<Variables> variables, std::shared_ptr<Variables> unequals)
  : _n(n)
  , _variables(variables)
  , _unequals(unequals)
{
  // "condition" means here one of the predicates that form a list of conditions that are connected by "and"
  const std::set<Node> subjects = _n->_pImpl->get_left(condition); // contains the subject of a fact
  const std::set<Node> targets  = _n->_pImpl->get_right(condition); // contains both the object of a fact and the node representing the fact
  
  std::set<Node> objects;
  std::set<Node> relations = _n->filter(targets, _n->core.IsA, _n->core.RelationTypeCategory, &objects);

  if (relations.size() > 0)
  {
    if (relations.size() > 1)
    {
      throw std::runtime_error("Unification found more than one relations for given condition, which makes no sense");
    }

    Node relation = *relations.begin();

    if (_n->_pImpl->is_var(relation))
    {
      _relation_list = _n->_pImpl->get_left(_n->core.RelationTypeCategory);
      _relation_list = _n->filter(_relation_list, [&](Node nd) { return _n->_pImpl->is_var(nd); });
      _relation_variable = relation;
    }
    else
    {
      _relation_list.insert(relation);

      if (relation == _n->core.Unequal)
      {
        for (Node subject : subjects)
          for (Node object : objects)
            (*_unequals)[subject] = object;
      }
    }
  }
}

std::shared_ptr<Variables> Unification::Start()
{
  if (_relation_list.size()==0)
    return std::make_shared<Variables>();

  _relation_index = _relation_list.begin();
  _fact_index     = _n->_pImpl->get_left(*_relation_index).begin(); // we know that result of get_left is valid, because node *_relation_index exists

  if (_relation_variable == 0 && *_relation_list.begin() == _n->core.Unequal)
    return std::make_shared<Variables>();
  else
    return Next();
}

std::shared_ptr<Variables> Unification::Next()
{
  if (_relation_variable == 0 && *_relation_list.begin() == _n->core.Unequal)
    return nullptr;

  for (; _relation_index != _relation_list.end(); ++_relation_index)
  {

  }
  
  return std::make_shared<Variables>();
}

std::shared_ptr<Variables> Unification::Unequals()
{
  return std::make_shared<Variables>();
}
