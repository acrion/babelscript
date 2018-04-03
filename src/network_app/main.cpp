#include "interactive.h"
#include <iostream>
#include <fstream>
#include <codecvt>

using namespace babelscript_console;
Interactive interactive;

int main(int argc, char** argv)
{
  try
  {
    std::wstring exit_command=L".exit";

    for (int i=1; i<argc; ++i)
    {
      std::cout << "Importing file " << argv[i] << "..." << std::endl;
      std::wifstream stream(argv[i]);

      if (stream.fail()) throw std::runtime_error(std::string("Could not open file '") + argv[i] + "'");

      for (std::wstring line; std::getline(stream, line); )
      {
        interactive.process(line);
      }
      interactive.run(true);
    }

    if (argc>1) std::cout << "Ready." << std::endl;
    std::cout << "babelscript " << Interactive::get_version() << std::endl;
    std::cout << std::endl;
    if (argc==1) std::cout << "You may specify script files that will be processed prior interactive mode." << std::endl;
    std::wcout << L" -- interactive mode - type " << exit_command << L" to exit --" << std::endl;
    std::cout << std::endl;
    for (std::wstring line; std::getline(std::wcin, line) && line != exit_command; )
    {
      if (line.empty())
        std::wcout << "type " << exit_command << L" to exit --" << std::endl;
      else
      {
        interactive.process(line);
        interactive.run(true);
      }
    }

#if 0
    n.set_name(n.core.RelationTypeCategory, L"Relationstyp", "de");
    n.set_name(n.core.Causes,               L"=>",           "de");
    n.set_name(n.core.And,                  L",",            "de");
    n.set_name(n.core.IsA,                  L"ist ein",      "de");
    n.set_name(n.core.Unequal,              L"ungleich",     "de");
    n.set_name(n.core.Contradiction,        L"Widerspruch",  "de");

    //n.fact(n.node(L"ist Eigenschaft von", "de"), n.node(L"ist Gegenteil von", "de"),   { n.node(L"hat die Eigenschaft", "de") });
    //n.fact(n.node(L"ist Teil von", "de"),        n.node(L"ist Gegenteil von", "de"),   { n.node(L"hat den Bestandteil", "de") });
    //n.fact(n.node(L"ist zum Beispiel", "de"),    n.node(L"ist Gegenteil von", "de"),   { n.node(L"ist ein",             "de") });
    n.fact(n.node(L"hat den Bestandteil", "de"), n.node(L"hat die Eigenschaft", "de"), { n.node(L"transitiv",           "de") });
    //n.fact(n.node(L"hat die Eigenschaft", "de"), n.node(L"hat die Eigenschaft", "de"), { n.node(L"transitiv",           "de") });
    //n.fact(n.node(L"ist ein", "de"),             n.node(L"hat die Eigenschaft", "de"), { n.node(L"transitiv",           "de") });

    {
      Node X = n.var();
      Node R = n.var();
      Node Y = n.var();
      Node Z = n.var();
      Node R_is_transitive = n.fact(R, n.node(L"hat die Eigenschaft", "de"), { n.node(L"transitiv", "de") });
      Node condition = n.condition(n.core.And, { R_is_transitive, n.fact(X, R, {Y}), n.fact(Y, R, {Z}) });
      n.fact(condition, n.core.Causes, { n.fact(X, R, {Z}) });
    }

    n.fact(n.node(L"Haus", "de"), n.node(L"hat den Bestandteil", "de"), { n.node(L"T\xfcr", "de") });
    n.fact(n.node(L"T\xfcr", "de"), n.node(L"hat den Bestandteil", "de"), { n.node(L"Griff", "de") });
    n.gen_dot(n.core.RelationTypeCategory, "test.dot");

    n.run();
    std::cout << "Ready." << std::endl;
    n.debug();
#endif
  }
  catch (std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
  }
}
