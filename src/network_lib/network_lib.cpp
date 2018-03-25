#include "network_lib.h"
#include "network_lib_impl.h"
#include "boost/filesystem.hpp"

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
  fact(core.IsA,     core.IsA, core.RelationTypeCategory);
  fact(core.Unequal, core.IsA, core.RelationTypeCategory);
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


void Network_lib::connect(Node a, Node b)
{
  _pImpl->connect(a,b);
}

Answer Network_lib::check_fact(Node source, Node relationType, Node target)
{
  Node relation = _pImpl->create_hash(relationType, source, target);

  if (   !_pImpl->is_var(source|relationType|target)
      && _pImpl->exists(relation)
      && check_fact(relation, core.IsA, core.RelationTypeCategory).is_correct())
  {
    return Answer(  _pImpl->probability(source, relation)
                  * _pImpl->probability(relation, target)
                  * _pImpl->probability(relation, relationType),
                  relation
                 );
  }
  else
  {
    return Answer(relation); // unknown
  }
}

Node Network_lib::fact(Node source, Node relationType, Node target, long double probability)
{
  if (source==target)
  {
    throw std::runtime_error("add_fact: source is identical to target");
  }

  if (   relationType != core.IsA
      && check_fact(relationType, core.IsA, core.RelationTypeCategory).is_wrong())
  {
    throw std::runtime_error("add_fact: node is known to be no relation");
  }

  Answer answer = check_fact(source, relationType, target);

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
    _pImpl->create(answer.relation());
    _pImpl->connect(source, answer.relation());
    _pImpl->connect(answer.relation(), target);
    _pImpl->connect(answer.relation(), relationType, probability);
  }

  return answer.relation();
}

Node Network_lib::fact(std::vector<Node> conditions, Node relation, Node deduction, long double probability)
{
  switch (conditions.size())
  {
  case 0: return deduction;
  case 1: return fact(conditions[0], relation, deduction, probability);
  default:
    Node and_relation = _pImpl->create_hash(core.And, conditions);
    _pImpl->create(and_relation);
    for (Node condition : conditions) _pImpl->connect(condition, and_relation);
    return fact(and_relation, relation, deduction, probability);
  }
}

void Network_lib::run()
{

}
