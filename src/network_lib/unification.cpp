#pragma once

#include <unification.h>

using namespace network;

Unification::Unification(Node condition, std::shared_ptr<Variables> variables, std::shared_ptr<Variables> unequals)
{

}

std::shared_ptr<Variables> Unification::Start()
{
  return std::make_shared<Variables>();
}

std::shared_ptr<Variables> Unification::Next()
{
  return std::make_shared<Variables>();
}

std::shared_ptr<Variables> Unification::Unequals()
{
  return std::make_shared<Variables>();
}
