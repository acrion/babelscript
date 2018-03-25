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

//  n.set_name(n.core.RelationTypeCategory, L"Relationstyp", "de");

//  n.add_fact(n.get_node("de", ""), n.get_node("de", ""), n.get_node("de", ""));
//  n.add_fact(n.get_node(""), n.get_node(""), n.get_node(""));
//  n.add_fact(n.get_node(""), n.get_node(""), n.get_node(""));
//  n.add_fact(n.get_node(""), n.get_node(""), n.get_node(""));
//  n.add_fact(n.get_node(""), n.get_node(""), n.get_node(""));
//  n.add_fact(n.get_node(""), n.get_node(""), n.get_node(""));
//  n.add_fact(n.get_node(""), n.get_node(""), n.get_node(""));
//  n.add_fact(n.get_node(""), n.get_node(""), n.get_node(""));

  n.debug();
}
