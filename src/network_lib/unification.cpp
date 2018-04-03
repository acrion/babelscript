#include <unification.h>
#include "network_lib_impl.h"
#include "utils.h"

using namespace network;

Unification::Unification(Network_lib* n, Node condition, Node parent, std::shared_ptr<Variables> variables, std::shared_ptr<Variables> unequals)
  : _n(n)
  , _parent(parent)
  , _variables(variables)
  , _unequals(unequals)
{
  // "condition" means here one of the predicates that form a list of conditions (which are connected by "and")

  std::unordered_set<Node> relations = _n->filter(condition, _n->core.IsA, _n->core.RelationTypeCategory);

  if (relations.size() == 1) // more than one relation for given condition makes no sense. _relation_list is empty, so Next() won't return anything
  {
    Node relation = *relations.begin(); // there is always only 1 relation
    _subject = _n->parse_fact(condition, _objects, _parent);

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

  _relation_index = _relation_list.begin();
}

bool Unification::increment_fact_index()
{
  do
  {
    if (!_initialized) // first call after construction. Condition _fact_index == decltype(_facts_of_current_relation->second)::iterator() causes _DEBUG_ERROR("map/set iterators incompatible") if false
    {
      _initialized = true;
      _facts_of_current_relation = _n->_pImpl->find_left(*_relation_index);

      if (_facts_of_current_relation == _n->_pImpl->right_end())
        return false; // there is a relation without any facts that use it (might happen we it has been explicitely defined via fact(r, core.IsA, core.RelationType))
      else
        _fact_index = _facts_of_current_relation->second.begin(); // used to iterate over all facts the have relation type *_relation_index
    }
    else if (++_fact_index == _facts_of_current_relation->second.end()) // increment and return false if we reached the end, so _relation_index will be incremented
    {
      return false;
    }
  } while (_n->_pImpl->get_left(*_fact_index).count(*_relation_index) == 1); // skip nodes that represent not relations of type *_relation_index, but relations having *_relation_index as subject (using bidirectional connection to the subject)

  return true;
}

std::shared_ptr<Variables> Unification::Next()
{
  if (_relation_list.size() == 0 || (_relation_variable == 0 && *_relation_list.begin() == _n->core.Unequal))
    return nullptr;

  do
  {
    if (   _relation_variable == 0
        || !_variables
        || utils::get(*_variables, _relation_variable, *_relation_index) == *_relation_index)
    {
      while (increment_fact_index())
      {
        std::unordered_set<Node> objects;
        Node subject = _n->parse_fact(*_fact_index, objects, *_relation_index); // todo: use _parent instead of *_relation_index!? or both?!

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

          return result;
        }
      }
    }
  } while (++_relation_index != _relation_list.end());
  
  return nullptr;
}

std::shared_ptr<Variables> Unification::Unequals()
{
  return _unequals;
}
