#include "interactive.hpp"
#include "reasoning.hpp"
#include "wikidata.hpp"
#include "stopwatch.hpp"
#include "utils.hpp"
#include <boost/tokenizer.hpp>
#include <boost/bimap.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>

using namespace babelscript::console;
using namespace babelscript::network;
using boost::tokenizer;
using boost::escaped_list_separator;

class Interactive::Impl
{
public:
  Impl() : _n(new Reasoning([](std::wstring str, bool)
    {
      std::wcout << str << std::endl;
    }))
  {
    _n->set_lang("en");
  }

  network::Node process_fact(const std::vector<std::wstring>& tokens, const std::map<std::vector<std::wstring>, Node>& relations, boost::bimap<std::wstring, Node>& variables);
  static bool is_var(std::wstring token);

  Reasoning* const _n;
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
  return Babelscript::get_version();
}

void Interactive::process(std::wstring line)
{
  try
  {
    if (boost::starts_with(line, "#")) return; // comment
    
    tokenizer<escaped_list_separator<wchar_t>, std::wstring::const_iterator, std::wstring > tok(line,
      escaped_list_separator<wchar_t>(L"\\", L" \t", L"\""));

    decltype(tok)::iterator it = tok.begin();

    while (it!=tok.end() && it->empty()) ++it;

    if (it == tok.end())
    {
      return; // empty line
    }

    if ((*it)[0] == L'.')
    {
      std::vector<std::wstring> cmd;
      while (it != tok.end())
      {
        if (!it->empty()) cmd.push_back(*it);
        ++it;
      }
      
      if (cmd[0] == L".lang")
      {
        if (cmd.size()<2) throw std::runtime_error("Command .lang: Missing language identifier");
        _pImpl->_n->set_lang(utils::str(cmd[1]));
      }
      else if (cmd[0] == L".name")
      {
        if (cmd.size() == 1) throw std::runtime_error("Command .name: Missing current name. Usage: .name <current name in " + _pImpl->_n->lang() + "> <language identifier> <name in that language>");
        if (cmd.size() == 2) throw std::runtime_error("Command .name: Missing language identifier. Usage: .name <current name in " + _pImpl->_n->lang() + "> <language identifier> <name in that language>");
        if (cmd.size() == 3) throw std::runtime_error("Command .name: Missing " + utils::str(cmd[2]) + " name of " + utils::str(cmd[1]) + ". Usage: .name <current name in " + _pImpl->_n->lang() + "> <language identifier> <name in that language>");
        _pImpl->_n->set_name(_pImpl->_n->node(cmd[1]), cmd[3], utils::str(cmd[2]));
      }
      else if (cmd[0] == L".wikidata")
      {
        if (cmd.size()<2) throw std::runtime_error("Command .wikidata: Missing json file name");

        std::wofstream log("wikidata.log");
        _pImpl->_n->set_print([&](std::wstring str, bool o)
        {
          log << str << std::endl;

          if (o)
          {
            std::wcout << str << std::endl;
          }
        });

        Wikidata wikidata(_pImpl->_n);
        StopWatch watch;
        watch.start();
        wikidata.import_json(cmd[1]);
        _pImpl->_n->print(L" Time needed for importing: " + std::to_wstring(static_cast<double>(watch.duration()) / 1000) + L"s", true);
        run(false);
        _pImpl->_n->print(L" Total time: " + std::to_wstring(static_cast<double>(watch.duration()) / 1000) + L"s", true);
        
        _pImpl->_n->set_print([](std::wstring str, bool)
        {
          std::wcout << str << std::endl;
        });
      }
      else if (cmd[0] == L".dot")
      {
        if (cmd.size()==1) throw std::runtime_error("Command .dot: Missing node name to visualize");
        Node nd = _pImpl->_n->get_node(cmd[1]);
        if (nd == 0) throw std::runtime_error("Command .dot: Unknown node '" + utils::str(cmd[1]) + "'");
        if (cmd.size()<3) throw std::runtime_error("Command .dot: Missing maximum depth");
        int max_depth = std::atoi(utils::str(cmd[2]).c_str());
        if (max_depth < 2) throw std::runtime_error("Command .dot: Maximum depth must be greater than 1");
        _pImpl->_n->gen_dot(nd, utils::str(cmd[1]) + ".dot", max_depth);
      }
      else
      {
        throw std::runtime_error(utils::str(L"Unknown command " + cmd[0]));
      }
    }
    else
    {
      boost::bimap<std::wstring, network::Node> variables;
      std::vector<std::wstring> tokens;
      const std::wstring And = _pImpl->_n->get_name(_pImpl->_n->core.And, _pImpl->_n->lang());
      const std::wstring Causes = _pImpl->_n->get_name(_pImpl->_n->core.Causes, _pImpl->_n->lang());
      bool is_rule = false;
      std::wstring assigns_to_var;
      std::wstring first_var = Impl::is_var(*it) ? L"" : *it;
      for (; it != tok.end(); ++it)
      {
        if (it->empty()) continue;

        std::wstring current;

        if (boost::algorithm::starts_with(*it, And))
        {
          current = it->substr(And.size());
          tokens.push_back(And);
          tokens.push_back(current);
        }

        if (boost::algorithm::ends_with(*it, And))
        {
          if (current.empty())
          {
            current = it->substr(0, it->size() - And.size());
            tokens.push_back(current);
          }
          tokens.push_back(And);
        }

        if (current.empty())
        {
          current = *it;
          tokens.push_back(current);
        }

        if (current == Causes)
        {
          if (is_rule)
            throw std::runtime_error("Line contains two times " + utils::str(_pImpl->_n->get_name(_pImpl->_n->core.Causes))); // this is just a parser restriction to distinguish between condition and deduction, might be useful to support it
          else
            is_rule = true;
        }
        else if (tokens.size() == 2 && *it == L"=" && !first_var.empty())
        {
          assigns_to_var = first_var;
          tokens.clear();
        }
      }

      // tokenize relation names to single words to prepare comparing them with the tokens (see process_fact)
      std::map<std::vector<std::wstring>, network::Node> relations;
      for (network::Node relation : _pImpl->_n->get_sources(_pImpl->_n->core.IsA, _pImpl->_n->core.RelationTypeCategory, true))
      {
        std::wstring name = _pImpl->_n->get_name(relation, _pImpl->_n->lang());
        if (!name.empty())
        {
          tokenizer<escaped_list_separator<wchar_t>, std::wstring::const_iterator, std::wstring > t(name,
            escaped_list_separator<wchar_t>(L"\\", L" \t", L"\""));

          std::vector<std::wstring> list;
          for (decltype(tok)::iterator to = t.begin(); it != t.end(); ++it) list.push_back(*to);
          relations[list] = relation;
        }
      }

      network::Node fact = 0;
      if (is_rule)
      {
        std::vector<decltype(tokens)> conditions, deductions;
        bool is_deduction = false;

        for (size_t i = 0; i < tokens.size(); ++i)
        {
          decltype(tokens) tokens_for_fact;
          while (i < tokens.size() && tokens[i] != And && tokens[i] != Causes)
            tokens_for_fact.push_back(tokens[i++]);

          if (tokens_for_fact.size() == 0)
          {
            throw std::runtime_error("Found empty condition or deduction in " + utils::str(line));
          }

          if (is_deduction)
            deductions.emplace_back(tokens_for_fact);
          else
            conditions.emplace_back(tokens_for_fact);

          if (i < tokens.size() && tokens[i] == Causes) is_deduction = true;
        }

        if (conditions.size() == 0) throw std::runtime_error("Found rule without condition in " + utils::str(line));
        if (deductions.size() == 0) throw std::runtime_error("Found rule without condition in " + utils::str(line));

        std::unordered_set<network::Node> condition_nodes;
        for (auto condition : conditions)
        {
          condition_nodes.insert(_pImpl->process_fact(condition, relations, variables));
        }

        network::Node combined_condition = conditions.size() == 1
          ? *condition_nodes.begin()
          : _pImpl->_n->condition(_pImpl->_n->core.And, condition_nodes);

        std::unordered_set<network::Node> deduction_list;

        for (auto deduction : deductions)
        {
          deduction_list.insert(_pImpl->process_fact(deduction, relations, variables));
        }

        fact = _pImpl->_n->fact(combined_condition, _pImpl->_n->core.Causes, deduction_list);
      }
      else
      {
        fact = _pImpl->process_fact(tokens, relations, variables);
      }

      if (!assigns_to_var.empty())
      {
        _pImpl->_n->set_name(fact, assigns_to_var, _pImpl->_n->lang());
      }

      std::wstring output;
      //_pImpl->_n->gen_dot(fact, "debug.dot", 5);
      _pImpl->_n->format_fact(output, _pImpl->_n->lang(), fact);
      _pImpl->_n->print(L"> " + output, false);
    }
  }
  catch (std::exception& ex)
  {
    throw std::runtime_error("Error in line \"" + utils::str(line) + "\": " + ex.what());
  }
}

babelscript::network::Node Interactive::Impl::process_fact(const std::vector<std::wstring>& tokens, const std::map<std::vector<std::wstring>, network::Node>& relations, boost::bimap<std::wstring, network::Node>& variables)
{
  class C
  {
    Interactive::Impl* _enclosing{ nullptr };

  public:
    C(Interactive::Impl* enclosing, network::Node node=0, std::wstring token=L"")
      : _enclosing(enclosing)
      , _node(node)
      , _token(token)
    {
    }
    network::Node _node{ 0 };
    std::wstring _token;

    static C grab(const std::vector<C>& list, size_t& i, const size_t max_index)
    {
      if (list[i]._node) return list[i++];
      C result(list[0]._enclosing);
      while (i <= max_index && !list[i]._node)
      {
        if (!result._token.empty()) result._token += L" ";
        result._token += list[i++]._token;
      }
      return result;
    }
    
    operator network::Node()
    {
      if (_node)
        return _node;
      else
      {
        if (_token.empty())
          throw std::logic_error("Internal error: empty token in process_fact::C");
        else
          return _node = _enclosing->_n->node(_token, _enclosing->_n->lang());
      }
    }
  };

  if (tokens.size() == 1)
  {
    if (*tokens.begin() == _n->get_name(_n->core.Contradiction))
    {
      return _n->core.Contradiction;
    }
    else
    {
      throw std::runtime_error(utils::str(L"Fact '" + *tokens.begin() + L"' consists of only 1 token, which is only allowed for contradiction '" + _n->get_name(_n->core.Contradiction) + L"'"));
    }
  }

  std::vector<C> combined;
  bool done_all=true;
  network::Node result=0;

  for (size_t i = 0; i < tokens.size(); ++i)
  {
    std::wstring token = tokens[i];

    if (is_var(token))
    {
      auto var = variables.left.find(token);
      if (var==variables.left.end())
      {
        Node node = _n->var();
        _n->set_name(node, token, _n->lang());
        variables.left.insert(boost::bimap<std::wstring, network::Node>::left_value_type(token, node));

        if (variables.left.find(token) == variables.left.end())
        {
          throw std::runtime_error(utils::str(L"Variable " + token + L" has been set to a different node."));
        }

        combined.push_back(C(this, node, token));
      }
      else
      {
        combined.push_back(C(this,var->second, token));
      }
    }
    else
    {
      size_t len=1;
      bool done=false;
      for (; len <= tokens.size()-i && !is_var(tokens[i]); ++len)
      {
        std::vector<std::wstring> test;
        test.insert(test.end(), tokens.begin()+i, tokens.begin()+i+len);
        auto relation = relations.find(test);
        if (relation != relations.end())
        {
          std::wstring connected;
#ifdef _DEBUG
          connected = utils::concatenate(test, L' ');
#endif
          combined.push_back(C(this,relation->second, connected));
          done = true;
          break;
        }
        else
        {
          std::wstring connected = utils::concatenate(test, L' ');
          network::Node node = _n->get_node(connected, _n->lang());
          if (node)
          {
            combined.push_back(C(this,node,connected));
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

  if (!done_all && combined.size() > 3)
  {
    // combined is longer than 3 elements, so aggregate it to get subject, predicate, object
    decltype(combined) temp;
    size_t i = 0;
    temp.emplace_back(C::grab(combined, i, combined.size() - 3));
    temp.emplace_back(C::grab(combined, i, combined.size() - 2));
    temp.emplace_back(C::grab(combined, i, combined.size() - 1));

    if (i != combined.size())
    {
      throw std::runtime_error(utils::str(L"cannot match fact or condition (use quotation marks?): " + temp[0]._token + L"  " + temp[1]._token + L"  " + temp[2]._token + L" ..."));
    }

    combined = std::move(temp);
  }

  std::unordered_set<Node> objects;
  for (size_t i = 2; i < combined.size(); ++i) objects.insert(combined[i]);
  result = _n->fact(combined[0], combined[1], objects);

  return result;
}

void Interactive::run(bool print_deductions)
{
  _pImpl->_n->run(print_deductions);
}


bool Interactive::Impl::is_var(std::wstring token)
{
  static const std::wstring variable_names(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ_");

  switch (token.size())
  {
  case 0:  return false;
  case 1:  return variable_names.find(*token.begin()) != std::wstring::npos;
  default: return *token.begin() == L'_';
  }
}
