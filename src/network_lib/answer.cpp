#include "answer.h"

using namespace network;

Answer::Answer(State state, long double probability, Node relation)
  : state(state)
  , _probability(std::min(1.0L, std::max(0.0L, probability)))
  , _relation(relation)
{
  if (state == State::Known && (_probability > 0 || _relation == 0))
  {
    throw std::runtime_error("Answer is known, but relation node is not set");
  }
}

Answer::Answer(long double probability, Node relation)
  : state(State::Known)
  , _probability(std::min(1.0L, std::max(0.0L, probability)))
  , _relation(relation)
{
  if (state == State::Known && _probability > 0 && _relation == 0)
  {
    throw std::runtime_error("Answer is known, but relation node is not set");
  }
}

Answer::Answer(Node relation)
  : state(State::Unknown)
  , _relation(relation)
{
}

Node Answer::relation()
{
  return _relation;
}

bool Answer::is_known()
{
  return state == State::Known;
}

bool Answer::is_correct()
{
  return state == State::Known && _probability > 0.5L;
}

bool Answer::is_wrong()
{
  return state == State::Known && _probability < 0.5L;
}

bool Answer::is_impossible()
{
  return state == State::Known && _probability == 0;
}
