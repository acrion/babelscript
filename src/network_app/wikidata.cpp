#include "wikidata.h"
#include "read_async.h"
#include "utils.h"
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
    static const std::wstring id_tag(L"\"id\":\"");
    size_t id0 = line.find(id_tag); if (id0 == std::wstring::npos) continue;
    size_t id1 = line.find(L"\"", id0 + id_tag.size() + 1);
    std::wstring id(line.substr(id0+id_tag.size(), id1 - id0 - id_tag.size()));

    Node current = _n->node(id, "wikidata");
    
    size_t language0;
    static const std::wstring language_tag(L"{\"language\":\"");
    
    while ((language0 = line.find(language_tag, id1+7)) != std::wstring::npos)
    {
      size_t language1 = line.find(L"\"", language0 + language_tag.size());
      std::string language = utils::str(line.substr(language0+language_tag.size(), language1 - language0 - language_tag.size()));

      static const std::wstring value_tag(L"\"value\":\"");
      id0 = line.find(value_tag, language1 + 1);
      id1 = line.find(L"\"", id0 + value_tag.size() + 1);
      std::wstring value(line.substr(id0+value_tag.size(), id1 - id0 - value_tag.size()));

      if (language == "en" || language=="de")
      {
        _n->set_name(current, value, language);

        std::wcout << id << L": " << utils::wstr(language) << L": " << value << std::endl;
      }
    }
  }
}
