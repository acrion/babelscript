#include "network_lib.h"
#include "network_lib_impl.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <iomanip>
#include <sstream>

using namespace network;

#ifdef _DEBUG
	#include <iostream>

	template<typename MapType>
	void Network_lib::print_map(const MapType& m, std::wstring strArrow)
	{
		typedef typename MapType::const_iterator const_iterator;
		for (const_iterator iter = m.begin(), iend = m.end(); iter != iend; ++iter)
		{
      if (!get_name(iter->first).empty() && iter->second.size()>0)
      {
        std::wcout << get_name(iter->first) << L" " << strArrow;

        for (Node a : iter->second)
        {
          std::wcout << L" " << get_name(a);
        }

        std::wcout << std::endl;
      }
		}
	}

	void Network_lib::debug()
	{
    std::cout << " === left ===" << std::endl;
    print_map(_pImpl->_left, L"-->");

    std::cout << std::endl << " === right ===" << std::endl;
    print_map(_pImpl->_right, L"<--");
	}
#else
	void Network_lib::debug()
	{
	}
#endif

Network_lib::Network_lib()
	: _pImpl{new Impl}
  , core({ _pImpl->create(), _pImpl->create(), _pImpl->create(), _pImpl->create(), _pImpl->create(), _pImpl->create()})
{
  fact(core.IsA, core.IsA, { core.RelationTypeCategory });
  fact(core.Unequal, core.IsA, { core.RelationTypeCategory });
  fact(core.Causes, core.IsA, { core.RelationTypeCategory });
}

Network_lib::~Network_lib()
{
  delete _pImpl;
}

Node Network_lib::node(std::wstring name, std::string lang)
{
	decltype(_pImpl->_names)::iterator namesInLang = _pImpl->_names.find(lang);
	decltype(namesInLang->second.right)::iterator node;

	if (   namesInLang == _pImpl->_names.end()
	    || (node = namesInLang->second.right.find(name)) == namesInLang->second.right.end()
	   )
	{
    Node newNode = _pImpl->create();
    _pImpl->_names[lang].insert(Impl::Names(newNode, name));
#if _DEBUG
    std::wcout << L"Node " << newNode << L" has name '" << name << L"' (" << std::wstring(lang.begin(), lang.end()) << L")" << std::endl;
#endif
    return newNode;
	}
	else
	{
		return node->second;
	}
}

Node Network_lib::var()
{
  return _pImpl->var();
}

std::wstring Network_lib::get_name(Node node, std::string lang)
{
  return _pImpl->get_name(node, _pImpl->_names.find(lang));
}


std::wstring Network_lib::get_name(Node node)
{
  for (decltype(_pImpl->_names)::iterator lang = _pImpl->_names.begin(); lang != _pImpl->_names.end(); ++lang)
  {
    std::wstring result = _pImpl->get_name(node, lang);

    if (!result.empty()) return result;
  }

  return L"";
}

std::string Network_lib::get_name_hex(Node node, bool prepend_num)
{
  std::string name = utils::str(get_name(node));
  
  if (name.empty())
  {
    if (_pImpl->is_var(node))
    {
      name = std::to_string(static_cast<int>(node));
    }
    else
    {
      std::ostringstream o;
      o << "0x" << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << node;
      name = o.str();
    }
  }
  else if (prepend_num && !_pImpl->is_hash(node) && !_pImpl->is_var(node))
  {
    name  = "(" + std::to_string(node) + ") " + name;
  }

  boost::replace_all(name, "\r\n", "\\n");
  boost::replace_all(name, "\n", "\\n");
  return name;
}


std::wstring Network_lib::Impl::get_name(Node node, decltype(_names)::iterator namesInLang)
{
  decltype(namesInLang->second.left)::iterator name;

  if (   namesInLang != _names.end()
      && (name = namesInLang->second.left.find(node)) != namesInLang->second.left.end()
     )
  {
    return name->second;
  }
  else
  {
    return L"";
  }
}


bool Network_lib::set_name(Node node, std::wstring newName, std::string lang)
{
  decltype(_pImpl->_names)::iterator namesInLang = _pImpl->_names.find(lang);
  decltype(namesInLang->second.left)::iterator name;

#if _DEBUG
  std::wcout << L"Node " << node << L" has name '" << newName << L"' (" << std::wstring(lang.begin(), lang.end()) << L")" << std::endl;
#endif

  if (   namesInLang != _pImpl->_names.end()
      && (name = namesInLang->second.left.find(node)) != namesInLang->second.left.end()
     )
  {
    return name->second == newName;
  }
  else
  {
    _pImpl->_names[lang].insert(Impl::Names(node, newName));
    return true;
  }
}


std::set<Node> Network_lib::get_sources(Node relationType, Node target, bool exclude_vars)
{
  std::set<Node> sources;
  
  for (Node relation: _pImpl->get_right(target))
    if (_pImpl->get_right(relation).count(relationType) == 1)
      for (Node source : _pImpl->get_left(relation))
        if (source!=target && (!exclude_vars || !_pImpl->is_var(source)))
          sources.insert(source);

  return sources;
}

std::set<Node> Network_lib::filter(std::set<Node> source, Node target)
{
  std::set<Node> result;

  for (Node nd : source)
  {
    if (_pImpl->get_right(nd).count(target) == 1)
    {
      result.insert(nd);
    }
  }

  return result;
}

std::set<Node> Network_lib::filter(std::set<Node> source, Node relationType, Node target, std::set<Node>* filtered_away)
{
  std::set<Node> result;

  for (Node nd : source)
  {
    bool match = false;
    std::set<Node> possible_relations = _pImpl->get_right(nd);
    for (Node relation : filter(possible_relations, relationType))
    {
      if (_pImpl->get_left(relation).count(target) == 1)
      {
        result.insert(nd);
        match = true;
      }
    }
    if (filtered_away && !match)
    {
      filtered_away->insert(nd);
    }
  }

  return result;
}

std::set<Node> Network_lib::filter(std::set<Node> source, std::function<bool(Node nd)> f)
{
  std::set<Node> result;

  for (Node nd : source)
  {
    if (f(nd)) result.insert(nd);
  }

  return result;
}

Answer Network_lib::check_fact(Node source, Node relationType, const std::set<Node>& targets)
{
  bool known = false;

  Node relation = _pImpl->create_hash(relationType, source, targets);

  if (_pImpl->exists(relation))
  {
    const std::set<Node>& connectedFromRelationType = _pImpl->get_right(relation);
    const std::set<Node>& connectedToRelationType = _pImpl->get_left(relation);

    known = connectedFromRelationType.count(source) == 1
      && connectedToRelationType.count(source) == 1 // source must be connected with and from <--> relation (to distinguish it from targets)
      && [&]() { for (Node t : targets) if (connectedToRelationType.count(t) == 0) return false; return true; }() // targets must all be connected with relation
      && [&]() { for (Node t : targets) if (connectedFromRelationType.count(t) == 1) return false; return true; }(); // no target must be connected from relation
  }

  if (known)
  {
    bool is_var = _pImpl->is_var(source | relationType)
      || [&]() { for (Node t : targets) if (_pImpl->is_var(t)) return true; return false; }();
    
    return Answer(
      is_var ? 0.5L : _pImpl->probability(relation, relationType), // probability
      relation);
  }
  else
  {
    return Answer(relation); // unknown
  }
}

Node Network_lib::fact(Node source, Node relationType, std::set<Node> targets, long double probability)
{
  Answer answer = check_fact(source, relationType, targets);

  if (answer.is_known())
  {
    if (answer.is_wrong() && probability>0.5L)
    {
      throw std::runtime_error("add_fact: this fact is known to be wrong");
    }
    else if (answer.is_correct() && probability<0.5L)
    {
      throw std::runtime_error("add_fact: this fact is known to be true");
    }
  }
  else
  {
    if (relationType != core.IsA) // note that the initial constructor call fact(core.IsA, core.IsA, core.RelationTypeCategory) is executed as intended
    {
      fact(relationType, core.IsA, { core.RelationTypeCategory });
    }

    _pImpl->create(answer.relation());
    _pImpl->connect(source, answer.relation());
    _pImpl->connect(answer.relation(), source);
    for (Node t:targets) _pImpl->connect(t, answer.relation());
    _pImpl->connect(answer.relation(), relationType, probability);

#ifdef _DEBUG
    bool has_name = !get_name(answer.relation()).empty() || !get_name(source).empty() || !get_name(relationType).empty();
    if (!has_name)
      for (Node t:targets)
        if (!get_name(t).empty())
        {
          has_name = true;
          break;
        }

    if (has_name)
    {
      std::string targets_name;
      for (Node t : targets)
      {
        if (!targets_name.empty()) targets_name += " and ";
        targets_name += get_name_hex(t, false);
      }
      set_name(answer.relation(), utils::wstr(get_name_hex(answer.relation(), false) + "\n" + get_name_hex(source, false) + " " + get_name_hex(relationType, false) + " " + targets_name));
    }
#endif
  }

  return answer.relation();
}

Node Network_lib::condition(Node op, std::set<Node> conditions)
{
  Node relation = _pImpl->create_hash(op, conditions);
  _pImpl->create(relation);
  _pImpl->connect(relation, op);
  for (Node condition : conditions) _pImpl->connect(condition, relation);
  return relation;
}


Node Network_lib::parse_rule(Node rule, std::set<Node>& deducations)
{
  Node condition = 0;
  deducations.clear();
  for (Node nd : _pImpl->get_left(rule))
  {
    if (nd != core.Causes)
    {
      if (_pImpl->get_left(nd).count(rule) == 1)
      {
        if (condition)
          return 0;
        else
          condition = nd;
      }
      else
        deducations.insert(nd);
    }
  }

  return condition;
}

void Network_lib::add_nodes(Node current, std::set<Node>& touched, const std::set<Node>& conditions, const std::set<Node>& deductions, std::ofstream& dot)
{
  if (touched.count(current) == 0)
  {
    touched.insert(current);
    std::string current_name = get_name_hex(current);

    if (_pImpl->is_var(current))
    {
      dot << "\"" << current_name << "\" [color=cornsilk2, style=filled]" << std::endl;
    }
    else if (conditions.find(current) != conditions.end())
    {
      dot << "\"" << current_name << "\" [color=lightskyblue, style=filled]" << std::endl;
    }
    else if (deductions.find(current) != deductions.end())
    {
      dot << "\"" << current_name << "\" [color=darkolivegreen2, style=filled]" << std::endl;
    }

    for (const Node& left : _pImpl->get_left(current))
    {
      Node hash = _pImpl->create_hash({ current,left });
      if (touched.count(hash) == 0)
      {
        touched.insert(hash);
        std::string left_name = get_name_hex(left);
        dot << "\"" << left_name << "\" -> \"" << current_name << "\"";

        if (_pImpl->find_left(left)->second.count(current) == 1)
        {
          dot << R"( [dir="both"])";
        }

        dot << ";" << std::endl;
      }
      add_nodes(left, touched, conditions, deductions, dot);
    }

    for (const Node& right : _pImpl->get_right(current))
    {
      Node hash = _pImpl->create_hash({ current,right });
      if (touched.count(hash) == 0)
      {
        touched.insert(hash);
        std::string right_name = get_name_hex(right);
        dot << "\"" << current_name << "\" -> \"" << right_name << "\"";

        if (_pImpl->find_right(right)->second.count(current) == 1)
        {
          dot << R"( [dir="both"])";
        }

        dot << ";" << std::endl;
      }
      add_nodes(right, touched, conditions, deductions, dot);
    }
  }
}

void Network_lib::gen_dot(Node start, std::string file_name)
{
  std::set<Node> conditions, deductions;
  
  for (Node rule : _pImpl->get_left(core.Causes))
  {
    std::set<Node> current_deductions;
    Node condition = parse_rule(rule, current_deductions);

    if (condition)
    {
      conditions.insert(condition);
      for (Node deduction : current_deductions)
      {
        deductions.insert(deduction);
      }
    }
  }

  std::set<Node> touched;
  std::ofstream dot(file_name, std::ios_base::out);

  dot << "digraph graphname{" << std::endl;

  add_nodes(start, touched, conditions, deductions, dot);
  
  dot << "}" << std::endl;
}
