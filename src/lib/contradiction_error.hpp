#pragma once

#include "utils.hpp"
#include <exception>

namespace babelscript
{
  namespace network
  {
    class contradiction_error : public std::exception
    {
    public:
      contradiction_error(Node fact, const Variables variables, Node parent)
        : std::exception()
        , _fact(fact)
        , _variables(variables)
        , _parent(parent)
      {

      }

      Node get_fact() const
      {
        return _fact;
      }

      const Variables& get_variables() const
      {
        return _variables;
      }

      Node get_parent() const
      {
        return _parent;
      }

    private:
      Node _fact;
      Variables _variables;
      Node _parent;
    };
  }
}
