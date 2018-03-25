#include "reasoning.h"
#include "unification.h"
#include "network_lib_impl.h"

using namespace network;

void Reasoning::run()
{
  do
  {
    _done = false;
    auto rules = _pImpl->find_left(core.Rule);

    if (rules != _pImpl->right_end())
    {
      for (Node rule : rules->second)
      {
        auto condition = _pImpl->get_left(rule);
        auto deduction = _pImpl->get_right(rule);

        if (condition.size()==1 && deduction.size() == 2)
        {
          auto d = deduction.begin();
          if (*d == core.Rule) ++d;
          
          _current_rule = rule;
          _next.clear();
          evaluate(RulePos({ rule, condition.end(), condition.begin(), *d }));
        }
      }
    }
  } while (_done);
}

void Reasoning::evaluate(RulePos rule)
{
  Node condition = *rule.index; // points to the relation (normally and) that connects the conditions
  auto current_op = _pImpl->get_right(condition);
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
    Unification u(condition, rule.variables, rule.unequals);
  }
}
