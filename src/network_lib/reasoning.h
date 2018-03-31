#pragma once

#include "network_lib_export.h"
#include "network_lib.h"
#include <vector>
#include <string>
#include <map>
#include <set>
#include <memory>

namespace network
{
  struct RulePos
  {
    Node node;
    std::set<Node>::iterator end;
    std::set<Node>::iterator index;
    std::shared_ptr<Variables> variables{ std::make_shared<Variables>() };
    std::shared_ptr<Variables> unequals{ std::make_shared<Variables>() };
  };
  
  class NETWORK_LIB_EXPORT Reasoning : public Network_lib
  {
  public:
    Reasoning(std::function<void(std::wstring)>);
    void run();

  private:
    void evaluate(RulePos rule);
    bool contradicts(const Variables& variables, const Variables& unequals);
    void deduce(const Variables& variables, Node parent);

    bool _done;
    Node _current_condition{ 0 };
    std::set<Node> _deductions;
    std::vector<RulePos> _next;
  };
}
