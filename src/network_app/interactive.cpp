#include "interactive.h"
#include "reasoning.h"
#include "utils.h"
#include <boost/tokenizer.hpp>
#include <boost/bimap.hpp>
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

  network::Node process_fact(const std::vector<std::wstring>& tokens, const std::map<std::vector<std::wstring>, Node>& relations, boost::bimap<std::wstring, Node>& variables);
  static bool is_var(std::wstring token);

  Reasoning* const _n;
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
  if (*it == L".lang")
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
    boost::bimap<std::wstring, network::Node> variables;
    std::vector<std::wstring> tokens;
    bool is_rule=false;
    std::wstring assigns_to_var;
    std::wstring first_var = Impl::is_var(*it) ? L"":*it;
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

    std::map<std::vector<std::wstring>, network::Node> relations;
    for (network::Node relation : _pImpl->_n->get_sources(_pImpl->_n->core.IsA, _pImpl->_n->core.RelationTypeCategory, true))
    {
      std::wstring name = _pImpl->_n->get_name(relation, _pImpl->_lang);
      if (!name.empty())
      {
        tokenizer<escaped_list_separator<wchar_t>,std::wstring::const_iterator, std::wstring > t(name,
          escaped_list_separator<wchar_t>(L"\\", L" \t", L"\""));

        std::vector<std::wstring> list;
        for (decltype(tok)::iterator to = t.begin(); it!=t.end(); ++it) list.push_back(*to);
        relations[list] = relation;
      }
    }

    network::Node fact=0;
    if (is_rule)
    {

    }
    else
    {
      fact = _pImpl->process_fact(tokens, relations, variables);
    }

    if (!assigns_to_var.empty())
    {
      _pImpl->_n->set_name(fact, assigns_to_var, _pImpl->_lang);
    }
  }
}

network::Node Interactive::Impl::process_fact(const std::vector<std::wstring>& tokens, const std::map<std::vector<std::wstring>, network::Node>& relations, boost::bimap<std::wstring, network::Node>& variables)
{
  class C
  {
  public:
    C(Interactive::Impl* enclosing, network::Node node, std::wstring token=L"")
      : _enclosing(enclosing)
      , _node(node)
      , _token(token)
    {
    }
    network::Node _node{ 0 };
    std::wstring _token;

    operator network::Node()
    {
      if (_node)
      {
        if (_token.empty())
          throw std::logic_error("Internal error: empty token in process_fact::C");
        else
          return _node = _enclosing->_n->node(_token, _enclosing->_lang);
      }
      else
        return _node;
    }

  private:
    Interactive::Impl* _enclosing{ nullptr };
  };

  std::vector<C> combined;
  bool done_all=true;
  network::Node result=0;

  for (int i=0; i<tokens.size(); ++i)
  {
    std::wstring token = tokens[i];

    if (is_var(token))
    {
      auto var = variables.left.find(token);
      if (var==variables.left.end())
      {
        Node node = _n->var();
        variables.left.insert(boost::bimap<std::wstring, network::Node>::left_value_type(token, node));
      }
      else
      {
        combined.push_back(C(this,var->second));
      }
    }
    else
    {
      int len=1;
      bool done=false;
      for (; len <= tokens.size()-i && !is_var(tokens[i]); ++len)
      {
        std::vector<std::wstring> test;
        test.insert(test.end(), tokens.begin()+i, tokens.begin()+i+len);
        auto relation = relations.find(test);
        if (relation != relations.end())
        {
          combined.push_back(C(this,relation->second));
          done = true;
          break;
        }
        else
        {
          std::wstring connected;
          for (std::wstring t : test)
          {
            if (!connected.empty()) connected += L" ";
            connected += t;
          }

          network::Node node = _n->get_node(connected, _lang);
          if (node)
          {
            combined.push_back(C(this,node));
            done = true;
            break;
          }
        }
      }

      if (done)
      {
        i += len-1;
      }
      else
      {
        combined.push_back(C(this,0,token));
        done_all = false;
      }
    }
  }

  if (combined.size()<3) throw std::runtime_error("A fact must consist of at least 3 tokens.");

  if (done_all || combined.size()==3)
  {
    std::set<Node> objects;
    for (int i=2; i<combined.size(); ++i) objects.insert(combined[i]);
    result = _n->fact(combined[0], combined[1], objects);
  }
  else
  {

  }

  return result;
}

void Interactive::run()
{
  _pImpl->_n->run();
}


bool Interactive::Impl::is_var(std::wstring token)
{
  if (token.size()==1)
    return true;
  else
    return !token.empty() && *token.begin()=='_';
}
