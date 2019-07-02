#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>
#include <map>
#include "utils.hpp"
#include <babelscript_export.h>

namespace babelscript
{
  namespace network
  {
    struct BABELSCRIPT_EXPORT Answer
    {
    protected:
      enum class State
      {
        Known, Unknown
      } state { State::Unknown };

      long double _probability{ 1 };
      Node _relation{0};

    public:
      Answer(State state, long double probability, Node relation = 0);
      Answer(long double probability, Node relation);
      Answer(Node relation);

      Node relation();
      bool is_known();
      bool is_correct();
      bool is_wrong();
      bool is_impossible();
    };
  }
}

