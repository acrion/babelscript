#pragma once

#include <babelscript_export.h>
#include "babelscript.hpp"
#include "stopwatch.hpp"
#include <vector>
#include <string>
#include <map>
#include <unordered_set>
#include <memory>

namespace babelscript
{
  namespace network
  {
    struct RulePos
    {
      Node node;
      std::unordered_set<Node>::iterator end;
      std::unordered_set<Node>::iterator index;
      std::shared_ptr<Variables> variables{ std::make_shared<Variables>() };
      std::shared_ptr<Variables> unequals{ std::make_shared<Variables>() };
    };
    
    class BABELSCRIPT_EXPORT Reasoning : public Babelscript
    {
    public:
      Reasoning(std::function<void(std::wstring, bool)>);
      void run(bool print_deductions);
      void apply_rule(const network::Node &rule, size_t thread_index);

    private:
      void evaluate(RulePos rule);
      bool contradicts(const Variables& variables, const Variables& unequals);
      void deduce(const Variables& variables, Node parent);

      bool _done;
      Node _current_condition{ 0 };
      std::unordered_set<Node> _deductions;
      std::vector<RulePos> _next;
      size_t _running{ 0 };
      bool _print_deductions{ true };
      bool _contradiction{ false };
      StopWatch _stop_watch;
      size_t _skipped{ 0 };
    };
  }
}

