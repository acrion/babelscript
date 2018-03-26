#pragma once

#include <unification.h>
#include "network_lib_impl.h"


using namespace network;

Unification::Unification(Network_lib* n, Node condition, std::shared_ptr<Variables> variables, std::shared_ptr<Variables> unequals)
  : _n(n)
{
  _n->_pImpl->right_end();
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
