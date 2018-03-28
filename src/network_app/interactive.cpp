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

  Reasoning* const _n;
  std::map<std::wstring, Node> _var;
};

Interactive::Interactive()
  : _pImpl(new Impl)
{
  _pImpl->_n->set_name(_pImpl->_n->core.RelationTypeCategory, L"R",  "en");
  _pImpl->_n->set_name(_pImpl->_n->core.Causes,               L"=>", "en");
  _pImpl->_n->set_name(_pImpl->_n->core.And,                  L",",  "en");
  _pImpl->_n->set_name(_pImpl->_n->core.IsA,                  L"E",  "en");
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

  if (it != tok.end())
  {
    if (*it == L".name")
    {
      if (++it == tok.end())
      {
        throw std::runtime_error("Command .name: Missing variable name and its name");
      }

      if (_pImpl->_var.find(*it)==_pImpl->_var.end())
      {
        throw std::runtime_error("Command .name: Unknown variable '"+utils::str(*it)+"'");
      }
    }
  }
}


void Interactive::run()
{
  _pImpl->_n->run();
}
