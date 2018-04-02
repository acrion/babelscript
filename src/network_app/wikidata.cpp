#include "wikidata.h"
#include "read_async.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>
#include <mutex>

using namespace babelscript_console;
using namespace network;
using boost::tokenizer;
using boost::escaped_list_separator;

class Wikidata::Impl
{
public:
  Impl()
  {
  }
};

Wikidata::Wikidata(network::Network_lib* n)
  : _pImpl(new Impl)
  , _n(n)
{
 _n->set_name(_n->core.RelationTypeCategory, L"Q18616576", "wikidata"); // Wikidata property
  
  
 // core.IsA is used for both P31 and P279 - we cannot use set_name for both. Maybe we should ignore P31 and use set_name(_n->core.IsA, L"P279", "wikidata") ?
 {
    Node X =_n->var();
    Node Y =_n->var();
    Node X_is_instance_of_Y =_n->fact(X,_n->node(L"P31", "wikidata"), { Y });
    Node X_is_a_Y =_n->fact(X,_n->core.IsA, { Y });
    _n->fact(X_is_instance_of_Y, _n->core.Causes, { X_is_a_Y });
  }
  
  {
    Node X =_n->var();
    Node Y =_n->var();
    Node X_is_subclass_of_Y =_n->fact(X,_n->node(L"P279", "wikidata"), { Y });
    Node X_is_a_Y =_n->fact(X,_n->core.IsA, { Y });
    _n->fact(X_is_subclass_of_Y, _n->core.Causes, { X_is_a_Y });
  }
}

Wikidata::~Wikidata()
{
  delete _pImpl;
}


void Wikidata::import_json(fs::path file_name)
{
  _n->_print(L"Importing file " + file_name.wstring());

  ReadAsync read_async(file_name);

  std::wstring line;
  while (read_async.get_line(line))
  {
    std::wcout << line << std::endl;
  }
}
