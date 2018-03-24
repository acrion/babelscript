#pragma once

namespace network
{
  struct Answer
  {
    enum class State
    {
      Known, Unknown
    } state { State::Unknown };

    long double probability{ 1 };

    Answer(State state, long double probability=1)
      : state(state)
      , probability(probability)
    {}

    Answer(long double probability)
      : state(State::Known)
      , probability(probability)
    {}

    operator bool()
    {
      return state==State::Known && probability > 0.5L;
    }
  };
}
