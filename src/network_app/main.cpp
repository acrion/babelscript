#include "network_lib.h"
#include <iostream>

using namespace network;

int main()
{
  Network_lib n;

  Node a = n.get_node("en", L"one");
  Node b = n.get_node("en", L"two");
  Node c = n.get_node("en", L"three");

  n.connect(a, b);
  n.connect(a, c);

  n.debug();
}
