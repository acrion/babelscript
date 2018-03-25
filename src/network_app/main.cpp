#include "network_lib.h"
#include <iostream>

using namespace network;

int main()
{
  Network_lib n;

  Node a = n.get_node(L"one", "en");
  Node b = n.get_node(L"two", "en");
  Node c = n.get_node(L"three", "en");

  n.connect(a, b);
  n.connect(a, c);

  n.set_name(n.core.RelationTypeCategory, L"Relationstyp", "de");
  n.set_name(n.core.Rule,                 L"=>",           "de");
  n.set_name(n.core.And,                  L",",            "de");
  n.set_name(n.core.IsA,                  L"ist ein",      "de");
  n.set_name(n.core.Unequal,              L"ungleich",     "de");
  n.set_name(n.core.Contradiction,        L"Widerspruch",  "de");

  n.add_fact(n.get_node(L"hat die Eigenschaft", "de"), n.get_node(L"ist ein", "de"), n.get_node(L"Relationstyp", "de"));
  //n.add_fact(n.get_node(L"", "de"), n.get_node(L"", "de"), n.get_node(L"", "de"));
  //n.add_fact(n.get_node(L"", "de"), n.get_node(L"", "de"), n.get_node(L"", "de"));
  //n.add_fact(n.get_node(L"", "de"), n.get_node(L"", "de"), n.get_node(L"", "de"));
  //n.add_fact(n.get_node(L"", "de"), n.get_node(L"", "de"), n.get_node(L"", "de"));
  //n.add_fact(n.get_node(L"", "de"), n.get_node(L"", "de"), n.get_node(L"", "de"));
  //n.add_fact(n.get_node(L"", "de"), n.get_node(L"", "de"), n.get_node(L"", "de"));
  //n.add_fact(n.get_node(L"", "de"), n.get_node(L"", "de"), n.get_node(L"", "de"));

  n.debug();
}
