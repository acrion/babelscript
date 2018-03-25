#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>
#include <map>
#include "utils.h"
#include "network_lib_export.h"

namespace network
{
  struct NETWORK_LIB_EXPORT Answer
  {
  protected:
    enum class State
    {
      Known, Unknown
    } state { State::Unknown };

    long double _probability{ 1 };
    Node _relation{0};

  public:
    Answer(State state, long double probability, Node relation=0)
      : state(state)
      , _probability(std::min(1.0L,std::max(0.0L,probability)))
      , _relation(relation)
    {
      if (state==State::Known && (_probability > 0 || _relation==0))
      {
        throw std::runtime_error("Answer is known, but relation node is not set");
      }
    }

    Answer(long double probability, Node relation)
      : state(State::Known)
      , _probability(std::min(1.0L,std::max(0.0L,probability)))
      , _relation(relation)
    {
      if (state==State::Known && (_probability > 0 || _relation==0))
      {
        throw std::runtime_error("Answer is known, but relation node is not set");
      }
    }

    Answer(Node relation)
      : state(State::Unknown)
      , _relation(relation)
    {
    }

    Node relation()
    {
      return _relation;
    }

    bool is_known()
    {
      return state==State::Known;
    }

    bool is_correct()
    {
      return state==State::Known && _probability > 0.5L;
    }

    bool is_wrong()
    {
      return state==State::Known && _probability < 0.5L;
    }

    bool is_impossible()
    {
      return state==State::Known && _probability == 0;
    }
  };
}
