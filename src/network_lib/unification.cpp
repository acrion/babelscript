#pragma once

#include <unification.h>
#include "network_lib_impl.h"
#include "utils.h"

using namespace network;

Unification::Unification(Network_lib* n, Node condition, Node parent, std::shared_ptr<Variables> variables, std::shared_ptr<Variables> unequals)
  : _n(n)
  , _parent(parent) // if _parent can be removed, RulePos.node can also be removed!
  , _variables(variables)
  , _unequals(unequals)
{
  // "condition" means here one of the predicates that form a list of conditions (which are connected by "and"),
  // so "condition" may point of core.And (or any other 

  std::set<Node> relations = _n->filter(condition, _n->core.IsA, _n->core.RelationTypeCategory);
  
  if (relations.size()==1) // more than one relation for given condition makes no sense. _relation_list is empty, so Next() won't return anything
  {
    Node relation = *relations.begin(); // there is always only 1 relation
    _subject = _n->parse_fact(condition, _objects);

    if (_n->_pImpl->is_var(relation))
    {
      // the relation is a variable, so fill _relation_list with all possible relations (excluding variables)
      _relation_list = _n->get_sources(_n->core.IsA, _n->core.RelationTypeCategory, true);
      _relation_variable = relation;
    }
    else
    {
      _relation_list.insert(relation); // leaving _relation_variable==0, which denotes that this condition specifies the relation (instead of using a variable for it)

      if (relation == _n->core.Unequal)
      {
        for (Node object : _objects)
          (*_unequals)[_subject] = object;
      }
    }
  }
}

std::shared_ptr<Variables> Unification::Start()
{
  if (_relation_list.size()==0)
    return std::make_shared<Variables>();

  if (_relation_variable == 0 && *_relation_list.begin() == _n->core.Unequal)
    return std::make_shared<Variables>();

  _relation_index = _relation_list.begin();
  auto left_nodes = _n->_pImpl->find_left(*_relation_index);

  if (left_nodes == _n->_pImpl->right_end())
  {
    return nullptr; // shouldn't happen since *_relation_index exists
  }

  _fact_index = left_nodes->second.begin();

  return Next();
}

std::shared_ptr<Variables> Unification::Next()
{
  if (_relation_list.size() == 0 || (_relation_variable == 0 && *_relation_list.begin() == _n->core.Unequal))
    return nullptr;

  while (true)
  {
    if (_relation_variable == 0 || !_variables || utils::get(*_variables, _relation_variable, *_relation_index) == *_relation_index)
    {
      while (_fact_index != _n->_pImpl->find_left(*_relation_index)->second.end())
      {
        std::set<Node> objects;
        Node subject = _n->parse_fact(*_fact_index, objects);

        //objects = _n->filter(objects, [&](Node nd)
        //{
        //  return nd != _parent;
        //});

        if (   !_n->_pImpl->is_var(subject)
            && !_n->_pImpl->is_var(*objects.begin())
            && utils::get(*_variables, _subject, subject) == subject
            && utils::get(*_variables, *_objects.begin(), *objects.begin()) == *objects.begin() // todo: what if more than one?
            && (   _n->_pImpl->is_var(_subject)
                || _subject == subject
               )
            && (   _n->_pImpl->is_var(*_objects.begin())
                || *_objects.begin() == *objects.begin() // todo: what if more than one?
               )
          )
        {
          std::shared_ptr<Variables> result = std::make_shared<Variables>();

          if (_variables->count(_subject)==0          && _n->_pImpl->is_var(_subject))          (*result)[_subject]          = subject;
          if (_variables->count(*_objects.begin())==0 && _n->_pImpl->is_var(*_objects.begin())) (*result)[*_objects.begin()] = *objects.begin(); // todo: what if more than one?
          if (_relation_variable != 0 && _variables->count(_relation_variable)==0) (*result)[_relation_variable] = *_relation_index;

          ++_fact_index;
          return result;
        }
        else
        {
          ++_fact_index;
        }
      }
    }

    if (++_relation_index == _relation_list.end())
    {
      break;
    }

    _fact_index = _n->_pImpl->find_left(*_relation_index)->second.begin(); // we know that result of get_left is valid, because node *_relation_index exists
  }
  
  return nullptr;
}

std::shared_ptr<Variables> Unification::Unequals()
{
  return _unequals;
}
