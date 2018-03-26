#include "reasoning.h"
#include "network_lib_impl.h"
#include "unification.h"

using namespace network;

void Reasoning::run()
{
  do
  {
    _done = false;
    for (Node rule : _pImpl->get_left(core.Causes))
    {
      auto deduction = _pImpl->get_right(rule);

      if (deduction.size() == 2)
      {
        auto condition = _pImpl->get_left(rule); // normally a set of 1 (the 'and' relation)
        auto d = deduction.begin();
        if (*d == core.Causes) ++d;
          
        _current_rule = rule;
        _next.clear();
        evaluate(RulePos({ rule, condition.end(), condition.begin(), *d }));
      }
    }
  } while (_done);
}

void Reasoning::evaluate(RulePos rule)
{
  Node condition = *rule.index; // points to the relation (normally 'and' or a variable) that connects the conditions
  auto current_op = _pImpl->get_right(condition); // get the set of Nodes the relation points to, which is its type. Empty if the relation is a variable.
  if (current_op.count(core.And) == 1)
  {
    auto condition_back_link = _pImpl->get_left(condition);
    if (condition_back_link.size() > 0)
    {
      if (rule.node != _current_rule)
      {
        RulePos next(rule);
        if (++next.index != next.end)
        {
          _next.push_back(next);
        }
      }

      evaluate(RulePos({condition, condition_back_link.end(), condition_back_link.begin(), rule.deduction, rule.variables, rule.unequals}));
    }
  }
  else
  {
    // here we run into for each part of a condition
    Unification u(this, condition, rule.variables, rule.unequals);

    for (std::shared_ptr<Variables> match=u.Start(); match; match=u.Next())
    {
      std::shared_ptr<Variables> joined          = utils::join(*rule.variables, *match);
      std::shared_ptr<Variables> joined_unequals = utils::join(*rule.unequals, *u.Unequals());

      bool done = false;
      if (rule.node != _current_rule)
      {
        RulePos next({rule.node, rule.end, rule.index, rule.deduction, joined, joined_unequals});
        if (++next.index != next.end)
        {
          evaluate(next);
          done = true;
        }
      }

      if (!done && _next.size() > 0)
      {
        RulePos next(_next.back());
        _next.pop_back();
        evaluate(next);
      }
      else if (!contradicts(*joined, *joined_unequals))
      {
        deduce(*joined);
      }
    }
  }
}

bool Reasoning::contradicts(const Variables& variables, const Variables& unequals)
{
  for (auto& var: unequals)
  {
    Node item1 = var.first;
    if (_pImpl->is_var(item1))
    {
      auto it = variables.find(item1);
      if (it != variables.end())
        item1 = it->second;
      else
        continue; // though this variable is included in the unequals, it is missing in the normal variables (meaningless unequal condition)
    }

    Node item2 = var.second;
    if (_pImpl->is_var(item2))
    {
      auto it = variables.find(item2);
      if (it != variables.end())
        item2 = it->second;
      else
        continue; // though this variable is included in the unequals, it is missing in the normal variables (meaningless unequal condition)
    }

    if (item1 == item2)
      return true; // contradiction, because item1 and item2 must be unequal
  }

  return false; // no contradiction
}

void Reasoning::deduce(const Variables& variables)
{

}
