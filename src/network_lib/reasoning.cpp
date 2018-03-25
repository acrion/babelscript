#include "reasoning.h"
#include "network_lib_impl.h"

using namespace network;

void Reasoning::run()
{
  do
  {
    _done = false;
    auto rules = _pImpl->find_left(core.Rule);

    if (rules != _pImpl->right_end())
    {
      for (Node rule : rules->second)
      {

      }
    }
  } while (_done);
}
