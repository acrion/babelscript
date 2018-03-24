#include "network_lib.h"
#include <iostream>

using namespace network;

int main()
{
	Network_lib network;

  uint64_t a = network.getNode("en", L"one");
  uint64_t b = network.getNode("en", L"two");
  uint64_t c = network.getNode("en", L"three");

	network.connect(a, b);
	network.connect(a, c);

	network.debug();
}
