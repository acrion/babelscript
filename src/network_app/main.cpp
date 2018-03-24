#include "network_lib.h"
#include <iostream>

using namespace network;

int main()
{
	Network_lib network;

  Node a = network.get_node("en", L"one");
  Node b = network.get_node("en", L"two");
  Node c = network.get_node("en", L"three");

	network.connect(a, b);
	network.connect(a, c);

	network.debug();
}
