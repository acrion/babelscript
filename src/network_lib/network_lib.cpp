#include "network_lib.h"
#include "network_lib_impl.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <iomanip>
#include <sstream>

using namespace network;

const std::string Network_lib::get_version()
{
  return "2.0";
}


Network_lib::Network_lib(std::function<void(std::wstring)> print)
	: _pImpl{new Impl}
  , _print(print)
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

Node Network_lib::var()
{
  return _pImpl->var();
}


void Network_lib::set_name(Node node, std::wstring name, std::string lang)
{
  if (lang.empty()) lang = _lang;
#if _DEBUG
  //std::wcout << L"Node " << node << L" has name '" << name << L"' (" << std::wstring(lang.begin(), lang.end()) << L")" << std::endl;
#endif

  std::lock_guard<std::mutex> lock(_pImpl->_mtx_name_of_node);
  std::lock_guard<std::mutex> lock2(_pImpl->_mtx_node_of_name);
  _pImpl->_node_of_name[lang][name] = node;
  _pImpl->_name_of_node[lang][node] = name;
}


Node Network_lib::node(std::wstring name, std::string lang)
{
  if (lang.empty()) lang = _lang;

  {
    std::lock_guard<std::mutex> lock(_pImpl->_mtx_node_of_name);
    auto& node_of_name = _pImpl->_node_of_name[lang];
    auto it = node_of_name.find(name);
    if (it != node_of_name.end())
    {
      return it->second;
    }
  }

  Node new_node = _pImpl->create();
  set_name(new_node, name, lang);
  return new_node;
}

std::wstring Network_lib::get_name(Node node, std::string lang, bool fallback)
{
  if (lang.empty()) lang = _lang;
  
  {
    std::lock_guard<std::mutex> lock(_pImpl->_mtx_name_of_node);
    auto& name_of_node = _pImpl->_name_of_node[lang];
    auto it = name_of_node.find(node);
    if (it != name_of_node.end())
    {
      return it->second;
    }
  }

  if (fallback)
  {
    std::lock_guard<std::mutex> lock(_pImpl->_mtx_name_of_node);
    for (auto l : _pImpl->_name_of_node)
    {
      auto it2 = l.second.find(node);
      if (it2 != l.second.end())
        return it2->second;
    }
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
      std::wstring output;
      format_fact(output, _lang, node);
      name = utils::str(output);
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


Node Network_lib::get_node(std::wstring name, std::string lang)
{
  if (lang.empty()) lang = _lang;
  std::lock_guard<std::mutex> lock(_pImpl->_mtx_node_of_name);
  auto node_of_name = _pImpl->_node_of_name.find(lang);
  if (node_of_name == _pImpl->_node_of_name.end())
  {
    return 0;
  }
  else
  {
    auto it = node_of_name->second.find(name);
    if (it == node_of_name->second.end())
    {
      return 0;
    }
    else
    {
      return it->second;
    }
  }
}

std::unordered_set<Node> Network_lib::get_sources(Node relationType, Node target, bool exclude_vars)
{
  std::unordered_set<Node> sources;
  
  for (Node relation: _pImpl->get_right(target))
    if (_pImpl->get_right(relation).count(relationType) == 1)
      for (Node source : _pImpl->get_left(relation))
        if (source!=target && (!exclude_vars || !_pImpl->is_var(source)))
          sources.insert(source);

  return sources;
}

std::unordered_set<Node> Network_lib::filter(std::unordered_set<Node> source, Node target)
{
  std::unordered_set<Node> result;

  for (Node nd : source)
  {
    if (_pImpl->get_right(nd).count(target) == 1)
    {
      result.insert(nd);
    }
  }

  return result;
}

std::unordered_set<Node> Network_lib::filter(Node fact, Node relationType, Node target)
{
  std::unordered_set<Node> source     = _pImpl->get_right(fact);
  std::unordered_set<Node> left_nodes = _pImpl->get_left(fact);
  std::unordered_set<Node> result;

  for (Node nd : source)
  {
    bool match = false;
    std::unordered_set<Node> possible_relations = _pImpl->get_right(nd);
    for (Node relation : filter(possible_relations, relationType))
    {
      if (   _pImpl->get_left(relation).count(target) == 1
          && left_nodes.count(nd)==0) // exclude the subject of the fact, since it is connected bidirectional. If <subject relationType target> is true, the subject would be included in the result by mistake
      {
        result.insert(nd);
        match = true;
      }
    }
  }

  return result;
}

std::unordered_set<Node> Network_lib::filter(std::unordered_set<Node> source, std::function<bool(Node nd)> f)
{
  std::unordered_set<Node> result;

  for (Node nd : source)
  {
    if (f(nd)) result.insert(nd);
  }

  return result;
}

Answer Network_lib::check_fact(Node source, Node relationType, const std::unordered_set<Node>& targets)
{
  bool known = false;

  Node relation = _pImpl->create_hash(relationType, source, targets);

  if (_pImpl->exists(relation))
  {
    const std::unordered_set<Node>& connectedFromRelationType = _pImpl->get_right(relation);
    const std::unordered_set<Node>& connectedToRelationType = _pImpl->get_left(relation);

    known = connectedFromRelationType.count(source) == 1
      && connectedToRelationType.count(source) == 1 // source must be connected with and from <--> relation (i. e. bidirectional, to distinguish it from targets)
      && [&]() { for (Node t : targets) if (connectedToRelationType.count(t) == 0) return false; return true; }() // targets must all be connected with relation
      && [&]() { for (Node t : targets) if (connectedFromRelationType.count(t) == 1) return false; return true; }(); // no target must be connected from relation
  }

  if (known)
  {
    return Answer(_pImpl->probability(relation, relationType), relation);
  }
  else
  {
    return Answer(relation); // unknown
  }
}

Node Network_lib::fact(Node source, Node relationType, std::unordered_set<Node> targets, long double probability)
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
    if (targets.count(relationType) == 1)
    {
      // 1 13 13
      // ~ is for example is for example <= (~  is opposite of  is for example), (is for example  ~  ->)
      throw std::runtime_error("add_fact: facts with same relation type and object are not supported.");
    }
    
    if (relationType != core.IsA) // note that the initial constructor call fact(core.IsA, core.IsA, core.RelationTypeCategory) is executed as intended
    {
      fact(relationType, core.IsA, { core.RelationTypeCategory });
    }

    if (_pImpl->exists(answer.relation()))
    {
      // check_fact can return KNOWN though answer.relation exists, which should not happen. Indicates corrupt database or hash collision.
      assert(false);
    }
    else
    {
      _pImpl->create(answer.relation());
    }

    _pImpl->connect(source, answer.relation());
    _pImpl->connect(answer.relation(), source);
    for (Node t:targets) _pImpl->connect(t, answer.relation());
    _pImpl->connect(answer.relation(), relationType, probability);
  }

  return answer.relation();
}

Node Network_lib::condition(Node op, std::unordered_set<Node> conditions)
{
  Node relation = _pImpl->create_hash(op, conditions);
  _pImpl->create(relation);
  _pImpl->connect(relation, op);
  for (Node condition : conditions) _pImpl->connect(condition, relation);
  return relation;
}


Node Network_lib::parse_fact(Node rule, std::unordered_set<Node>& objects, Node parent)
{
  Node subject = 0; // 0 means failure
  objects.clear();
  bool empty_subject = false;
  for (Node nd : _pImpl->get_left(rule))
  {
    if (_pImpl->get_left(nd).count(rule) == 1)
    {
      if (_pImpl->get_right(nd).count(core.Causes) == 0) // TODO: this might be not sufficient if rule is a sub-condition!?
      {
        assert(nd != parent); // indicates corrupt database
        
        if (subject)
          empty_subject = true; // there may be only 1 subject
        else
          subject = nd;
      }
    }
    else
    {
      assert(nd != parent); // indicates corrupt database
      objects.insert(nd);
    }
  }

  return empty_subject ? 0:subject;
}

Node Network_lib::parse_relation(Node rule)
{
  Node relation = 0; // 0 means failure
  Node subject = 0;
  for (Node nd : _pImpl->get_right(rule))
  {
    if (check_fact(nd, core.IsA, { core.RelationTypeCategory }).is_correct())
    {
      if (_pImpl->get_right(nd).count(rule) == 1) // in case nd is the subject of the rule, it may be also a relation, but not the one of the current rule so exclude it by checking for bidirectional connection
        subject = nd; // the rule has a subject that is a relation. We don't know yet, if it is a rule that has same subject and predicate.
      else if (relation)
        return 0; // there may be only 1 relation
      else
        relation = nd;
    }
  }

  if (relation == 0)
  {
    // Since we exclude setting relation to the subject of the rule, now that we have a rule without a relation, it must be a rule where subject and relation are identical.
    relation = subject;
  }

  return relation;
}

void Network_lib::format_fact(std::wstring& result, std::string lang, Node fact, const Variables& variables, Node parent, std::shared_ptr<std::unordered_set<Node>> history)
{
  if (!history) history = std::make_shared<std::unordered_set<Node>>();

  if (history->find(fact) != history->end())
  {
    result = L"?";
    return;
  }
  
  std::unordered_set<Node> objects;
  Node subject = parse_fact(fact, objects, parent);

  bool is_condition = _pImpl->get_right(fact).count(core.And) == 1; // if fact is a condition node (having relation type core.And), there is no subject
  if (subject == 0 && !is_condition)
  {
    result = L"?";
    return;
  }

  history->insert(fact);
  std::wstring subject_name, relation_name;

  if (!is_condition || subject)
  {
    subject = utils::get(variables, subject);
    subject_name = subject ? get_name(subject, lang, true) : (is_condition ? L"" : L"?");
    if (subject_name.empty())
    {
      format_fact(subject_name, lang, subject, variables, fact, history);
      subject_name = L"(" + subject_name + L")";
    }

    Node relation = parse_relation(fact);
    relation = utils::get(variables, relation);
    relation_name = relation ? get_name(relation, lang, true) : L"?";
    if (relation_name.empty())
    {
      format_fact(relation_name, lang, relation, variables, fact, history);
      relation_name = L"(" + relation_name + L")";
    }
  }

  std::wstring objects_name;
  for (Node object : objects)
  {
    object = utils::get(variables, object);
    if (!objects_name.empty()) objects_name += get_name(core.And, lang, true) + L" ";
    std::wstring object_name = object ? get_name(object, lang, true) : L"?";
    if (object_name.empty())
    {
      format_fact(object_name, lang, object, variables, fact, history);
      object_name = L"(" + object_name + L")";
    }
    objects_name += object_name;
  }
  if (objects_name.empty()) objects_name = L"?";

  result = subject_name + L"  " + relation_name + L"  " + objects_name;
  boost::replace_all(result, L"\r\n", L" --- ");
  boost::replace_all(result, L"\n", L" --- ");
  boost::trim(result);
}

void Network_lib::add_nodes(Node current, std::unordered_set<Node>& touched, const std::unordered_set<Node>& conditions, const std::unordered_set<Node>& deductions, std::ofstream& dot, int max_depth)
{
  if (--max_depth > 0 && touched.count(current) == 0)
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
      add_nodes(left, touched, conditions, deductions, dot, max_depth);
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
      add_nodes(right, touched, conditions, deductions, dot, max_depth);
    }
  }
}

void Network_lib::gen_dot(Node start, std::string file_name, int max_depth)
{
  std::unordered_set<Node> conditions, deductions;
  
  for (Node rule : _pImpl->get_left(core.Causes))
  {
    std::unordered_set<Node> current_deductions;
    Node condition = parse_fact(rule, current_deductions);

    if (condition && condition!=core.Causes)
    {
      conditions.insert(condition);
      for (Node deduction : current_deductions)
      {
        deductions.insert(deduction);
      }
    }
  }

  std::unordered_set<Node> touched;
  std::ofstream dot(file_name, std::ios_base::out);

  dot << "digraph graphname{" << std::endl;

  add_nodes(start, touched, conditions, deductions, dot, max_depth);
  
  dot << "}" << std::endl;
}
