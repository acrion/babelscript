#include "reasoning.h"
#include <iostream>

using namespace network;

int main()
{
  Reasoning n;

  Node a = n.node(L"one", "en");
  Node b = n.node(L"two", "en");
  Node c = n.node(L"three", "en");

  n.connect(a, b);
  n.connect(a, c);

  n.set_name(n.core.RelationTypeCategory, L"Relationstyp", "de");
  n.set_name(n.core.Rule,                 L"=>",           "de");
  n.set_name(n.core.And,                  L",",            "de");
  n.set_name(n.core.IsA,                  L"ist ein",      "de");
  n.set_name(n.core.Unequal,              L"ungleich",     "de");
  n.set_name(n.core.Contradiction,        L"Widerspruch",  "de");

  n.fact(n.node(L"ist Eigenschaft von", "de"), n.node(L"ist Gegenteil von",   "de"), n.node(L"hat die Eigenschaft", "de"));
  n.fact(n.node(L"ist Teil von",        "de"), n.node(L"ist Gegenteil von",   "de"), n.node(L"hat den Bestandteil", "de"));
  n.fact(n.node(L"ist zum Beispiel",    "de"), n.node(L"ist Gegenteil von",   "de"), n.node(L"ist ein",             "de"));
  n.fact(n.node(L"hat den Bestandteil", "de"), n.node(L"hat die Eigenschaft", "de"), n.node(L"transitiv",           "de"));
  n.fact(n.node(L"hat die Eigenschaft", "de"), n.node(L"hat die Eigenschaft", "de"), n.node(L"transitiv",           "de"));
  n.fact(n.node(L"ist ein",             "de"), n.node(L"hat die Eigenschaft", "de"), n.node(L"transitiv",           "de"));

  {
    Node X = n.var();
    Node R = n.var();
    Node Y = n.var();
    Node Z = n.var();
    Node R_is_transitive = n.fact(R, n.node(L"hat die Eigenschaft", "de"), n.node(L"transitiv", "de"));
    n.fact({ R_is_transitive, n.fact(X, R, Y), n.fact(Y, R, Z) }, n.core.Rule, n.fact(X, R, Z));
  }

  try
  {
    n.run();
    std::cout << "Ready." << std::endl;
  }
  catch (std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
  }

  n.debug();
}
