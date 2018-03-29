#include "interactive.h"
#include "reasoning.h"
#include "utils.h"
#include <boost/tokenizer.hpp>
#include <iostream>

using namespace babelscript_console;
using namespace network;
using boost::tokenizer;
using boost::escaped_list_separator;

class Interactive::Impl
{
public:
  Impl() : _n(new Reasoning([](std::wstring str)
    {
      std::wcout << str << std::endl;
    }))
  {}

  network::Node process_fact(const std::vector<std::wstring>& tokens, const std::vector<std::vector<std::wstring>>& relations);

  Reasoning* const _n;
  std::map<std::wstring, Node> _var;
  std::string _lang{"en"};
};

Interactive::Interactive()
  : _pImpl(new Impl)
{
  _pImpl->_n->set_name(_pImpl->_n->core.RelationTypeCategory, L"->", "en");
  _pImpl->_n->set_name(_pImpl->_n->core.Causes,               L"=>", "en");
  _pImpl->_n->set_name(_pImpl->_n->core.And,                  L",",  "en");
  _pImpl->_n->set_name(_pImpl->_n->core.IsA,                  L"~",  "en");
  _pImpl->_n->set_name(_pImpl->_n->core.Unequal,              L"!=", "en");
  _pImpl->_n->set_name(_pImpl->_n->core.Contradiction,        L"!",  "en");
}

Interactive::~Interactive()
{
  delete _pImpl;
}

std::string Interactive::get_version()
{
  return Network_lib::get_version();
}

void Interactive::process(std::wstring line)
{
  tokenizer<escaped_list_separator<wchar_t>,std::wstring::const_iterator, std::wstring > tok(line,
    escaped_list_separator<wchar_t>(L"\\", L" \t", L"\""));

  decltype(tok)::iterator it = tok.begin();
  if (*it == L".name")
  {
    if (++it == tok.end()) throw std::runtime_error("Command .name: Missing variable name and its value");
    auto var = _pImpl->_var.find(*it);
    if (var==_pImpl->_var.end()) throw std::runtime_error("Command .name: Unknown variable '"+utils::str(*it)+"'");
    if (++it == tok.end()) throw std::runtime_error("Command .name: Missing value for variable "+utils::str(var->first));
    _pImpl->_n->set_name(var->second, *it, _pImpl->_lang);
  }
  else if (*it == L".lang")
  {
    if (++it == tok.end()) throw std::runtime_error("Command .lang: Missing language identifier");
    _pImpl->_lang = utils::str(*it);
  }
  else if (*it == L".dot")
  {
    if (++it == tok.end()) throw std::runtime_error("Command .dot: Missing output file name");
    _pImpl->_n->gen_dot(_pImpl->_n->core.RelationTypeCategory, utils::str(*it));
  }
  else
  {
    std::vector<std::wstring> tokens;
    bool is_rule=false;
    std::wstring assigns_to_var;
    std::wstring first_var = is_var(*it) ? L"":*it;
    for (; it != tok.end(); ++it)
    {
      tokens.push_back(*it);
      if (*it == _pImpl->_n->get_name(_pImpl->_n->core.Causes, _pImpl->_lang))
      {
        if (is_rule)
          throw std::runtime_error("Line contains two times " + utils::str(_pImpl->_n->get_name(_pImpl->_n->core.Causes)));
        else
          is_rule = true;
      }
      else if (tokens.size()==2 && *it==L"=" && !first_var.empty())
      {
        assigns_to_var = first_var;
        tokens.clear();
      }
    }

    std::vector<std::vector<std::wstring>> relations;
    for (network::Node relation : _pImpl->_n->get_sources(_pImpl->_n->core.IsA, _pImpl->_n->core.RelationTypeCategory, true))
    {
      std::wstring name = _pImpl->_n->get_name(relation, _pImpl->_lang);
      if (!name.empty())
      {
        tokenizer<escaped_list_separator<wchar_t>,std::wstring::const_iterator, std::wstring > t(name,
          escaped_list_separator<wchar_t>(L"\\", L" \t", L"\""));

        std::vector<std::wstring> list;
        for (decltype(tok)::iterator it = t.begin(); it!=t.end(); ++it) list.push_back(*it);
        relations.push_back(list);
      }
    }

    network::Node fact=0;
    if (is_rule)
    {

    }
    else
    {
      fact = _pImpl->process_fact(tokens, relations);
    }

    if (!assigns_to_var.empty())
    {
      _pImpl->_var[assigns_to_var] = fact;
      _pImpl->_n->set_name(fact, assigns_to_var, _pImpl->_lang);
    }
  }
}

network::Node Interactive::Impl::process_fact(const std::vector<std::wstring>& tokens, const std::vector<std::vector<std::wstring>>& relations)
{
    return 0;
}

void Interactive::run()
{
  _pImpl->_n->run();
}


bool Interactive::is_var(std::wstring token)
{
  if (token.size()==1)
    return true;
  else
    return !token.empty() && *token.begin()=='_';
}
