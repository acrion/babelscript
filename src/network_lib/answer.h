#pragma once

#include <algorithm>

namespace network
{
  using Node = size_t;

  struct Answer
  {
  protected:
    enum class State
    {
      Known, Unknown
    } state { State::Unknown };

    long double probability{ 1 };
    Node relation{0};

  public:
    Answer(State state, long double probability, Node relation=0)
      : state(state)
      , probability(std::min(1.0L,std::max(0.0L,probability)))
      , relation(relation)
    {
      if (state==State::Known && (probability > 0 || relation==0))
      {
        throw std::runtime_error("Answer is known, but relation node is not set");
      }
    }

    Answer(long double probability, Node relation)
      : state(State::Known)
      , probability(std::min(1.0L,std::max(0.0L,probability)))
      , relation(relation)
    {
      if (state==State::Known && (probability > 0 || relation==0))
      {
        throw std::runtime_error("Answer is known, but relation node is not set");
      }
    }

    bool is_known()
    {
      return state==State::Known;
    }

    bool is_correct()
    {
      return state==State::Known && probability > 0.5L;
    }

    bool is_wrong()
    {
      return state==State::Known && probability < 0.5L;
    }

    bool is_impossible()
    {
      return state==State::Known && probability == 0;
    }

    static const Answer Unknown;
  };
}
