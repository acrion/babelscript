#include "reasoning.hpp"
#include "babelscript_impl.hpp"
#include "unification.hpp"
#include <thread>

using namespace babelscript::network;

Reasoning::Reasoning(std::function<void(std::wstring, bool)> print)
  : Babelscript(print)
{
}

//#define PARALLEL_REASONING

void Reasoning::run(bool print_deductions)
{
  _print_deductions = print_deductions;
  _skipped = 0;

  do
  {
    std::vector<std::thread> threads;

    _done = false;
    for (Node rule : _pImpl->get_left(core.Causes))
    {
#ifndef PARALLEL_REASONING
      apply_rule(rule, 0);
#else
      size_t i = 0;
      for (i = 0; i < threads.size(); ++i)
        if (!(_running & 1ull << i))
          break;

      _running = _running | 1ull << i;
      std::thread t(&Reasoning::apply_rule, this, rule, i);
      if (i < threads.size())
      {
        if (threads[i].joinable()) threads[i].join();
        threads[i] = std::move(t);
      }
      else
      {
        threads.emplace_back(std::move(t));
        print(L"Running " + std::to_wstring(threads.size()) + L" threads for applying rules...", true);
      }
#endif
    }

    for (std::thread& t : threads)
      if (t.joinable()) t.join();

  } while (_done);

  _stop_watch.stop();
  if (_skipped > 0) print(L" (skipped " + std::to_wstring(_skipped) + L" deductions)", true);
}

void Reasoning::apply_rule(const network::Node &rule, size_t thread_index)
{
  Node condition = parse_fact(rule, _deductions);

  if (condition && condition != core.Causes && _deductions.size() > 0)
  {
    std::unordered_set<Node> conditions;
    conditions.insert(condition);

    _current_condition = condition;
    _next.clear();
    evaluate(RulePos({ rule, conditions.end(), conditions.begin() }));
  }

  _running = _running & ~(1ull << thread_index);
}

void Reasoning::evaluate(RulePos rule)
{
  Node condition = *rule.index; // points to the relation: normally 'and', connecting several conditions, or a the relation of a fact (which may consist of variables)
  auto current_op = _pImpl->get_right(condition); // get the set of Nodes the relation points to, which is its type. Empty if the relation is a variable.

  // condition points to
  //    (1) the rule itself (i. e. the relaton node pointing to core.Causes), and
  //    (2) the type of relation, which may be any relation type (i. e. any node pointing having a core.IsA relation to core.RelationTypeCategory),
  //        but especially core.And (which makes this rule node a set of conditions)
  if (current_op.count(core.And) == 1)
  {
    auto condition_back_link = filter(_pImpl->get_left(condition), [&](Node nd) {return nd != rule.node; });
    if (condition_back_link.size() > 0)
    {
      RulePos next(rule);
      if (++next.index != next.end) // if we are on first recursion level, this will always fail, since the top list of conditions always has length 1
      {
        _next.push_back(next);
      }

      evaluate(RulePos({condition, condition_back_link.end(), condition_back_link.begin(), rule.variables, rule.unequals}));
    }
  }
  else
  {
    // here we run into for each part of a condition
    Unification u(this, condition, rule.node, rule.variables, rule.unequals);

    while (std::shared_ptr<Variables> match=u.Next())
    {
      std::shared_ptr<Variables> joined          = utils::join(*rule.variables, *match);
      std::shared_ptr<Variables> joined_unequals = utils::join(*rule.unequals, *u.Unequals());

      RulePos next({rule.node, rule.end, rule.index, joined, joined_unequals});
      if (++next.index != next.end) // if we are on first recursion level, this will always fail, since the top list of conditions always has length 1
      {
        evaluate(next);
      }
      else if (_next.size() > 0)
      {
        next = RulePos(_next.back());
        _next.pop_back();
        evaluate(next);
      }
      else if (!contradicts(*joined, *joined_unequals) && joined->size()>0)
      {
        deduce(*joined, rule.node);
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

void Reasoning::deduce(const Variables& variables, Node parent)
{
  for (Node deduction : _deductions)
  {
    if (deduction == core.Contradiction)
    {
      if (_print_deductions)
      {
        std::wstring input,output;
        format_fact(input, _lang, _current_condition, variables, parent);
        print(L"! <= " + input, true);
      }
      throw std::runtime_error("Contradiction!");
    }

    std::unordered_set<Node> relations = filter(deduction, core.IsA, core.RelationTypeCategory);

    if (relations.size() == 1) // more than one relation for given condition makes no sense. _relation_list is empty, so Next() won't return anything
    {
      Node rel = _pImpl->is_var(*relations.begin())
        ? utils::get(variables, *relations.begin(), 0ull)
        : *relations.begin();

      if (rel)
      {
        std::unordered_set<Node> var_targets;
        Node var_source = parse_fact(deduction, var_targets, parent);

        if (var_targets.size() > 0)
        {
          Node source = _pImpl->is_var(var_source)
            ? utils::get(variables, var_source, 0ull)
            : var_source;

          if (source)
          {
            std::unordered_set<Node> targets;
            bool done = true;
            for (Node var_t : var_targets)
            {
              Node t = _pImpl->is_var(var_t)
                ? utils::get(variables, var_t, 0ull)
                : var_t;

              if (t)
              {
                targets.insert(t);
              }
              else
              {
                done = false;
                break;
              }
            }

            if (done)
            {
              Answer answer = check_fact(source, rel, targets);

              if (answer.is_wrong())
              {
                throw std::runtime_error("Deduction is known to be wrong.");
              }
              else if (!answer.is_known()
                && targets.count(rel) == 0) // ignore deductions with same relation and target type as we do not support these
              {
                try
                {
                  Node d = fact(source, rel, targets);

                  bool do_print = _print_deductions;
                  if (!do_print)
                  {
                    if (_stop_watch.is_running())
                    {
                      if (_stop_watch.duration() >= 1000)
                      {
                        do_print = true;
                        _stop_watch.start();
                      }
                      else
                      {
                        ++_skipped;
                      }
                    }
                    else
                    {
                      do_print = true;
                      _stop_watch.start();
                    }
                  }

                  if (do_print)
                  {
                    if (_skipped > 0) print(L" (skipped " + std::to_wstring(_skipped) + L" deductions)", true);
                    _skipped = 0;
                    _stop_watch.start();
                    std::wstring input,output;
                    format_fact(input, _lang, _current_condition, variables, parent);
                    format_fact(output, _lang, d, {}, parent);
                    print(output + L" <= " + input, true);
                  }

                  _done = true;
                }
                catch (std::exception& ex)
                {
                  throw std::runtime_error(std::string("Deduction cannot be added: ") + ex.what());
                }
              }
            }
          }
        }
      }
    }
  }
}
