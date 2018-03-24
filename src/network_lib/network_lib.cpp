#include "network_lib.h"
#include "network_lib_impl.h"
#include "boost/filesystem.hpp"

using namespace network;

#ifdef _DEBUG
	#include <iostream>

	template<typename MapType>
	void Network_lib::print_map(const MapType& m)
	{
		typedef typename MapType::const_iterator const_iterator;
		for (const_iterator iter = m.begin(), iend = m.end(); iter != iend; ++iter)
		{
      if (!get_name(iter->first).empty() && iter->second.size()>0)
      {
        std::wcout << get_name(iter->first) << L" -->";

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
    print_map(_pImpl->_left);

    std::cout << std::endl << " === right ===" << std::endl;
    print_map(_pImpl->_right);
	}
#else
	void Network_lib::debug()
	{
	}
#endif

Network_lib::Network_lib()
	: _pImpl{new Impl}
{
  add_fact(_pImpl->_p.IsA,     _pImpl->_p.IsA, _pImpl->_p.RelationTypeCategory);
  add_fact(_pImpl->_p.Unequal, _pImpl->_p.IsA, _pImpl->_p.RelationTypeCategory);
}

Network_lib::~Network_lib()
{
	delete _pImpl;
}

Node Network_lib::get_node(std::string lang, std::wstring name)
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

std::wstring Network_lib::get_name(Node node, std::string lang)
{
	decltype(_pImpl->_names)::iterator namesInLang = _pImpl->_names.find(lang);
	decltype(namesInLang->second.left)::iterator name;

	if (   namesInLang != _pImpl->_names.end()
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

  if (   _pImpl->is_connected(source, relation) > 0 // subject
      && _pImpl->is_connected(target, relation) < 0 // object
      && check_fact(relation, _pImpl->_p.IsA, _pImpl->_p.RelationTypeCategory))
  {
    return Answer(  _pImpl->probability(source, relationType)
                  * _pImpl->probability(target, relationType)
                  * _pImpl->probability(relationType, _pImpl->_p.RelationTypeCategory)
                 );
  }
  else
  {
    return Answer(Answer::State::Unknown);
  }
}

void Network_lib::add_fact(Node source, Node relationType, Node target, long double probability)
{
  if (source==target)
  {
    throw std::runtime_error("add_fact: source is identical to target");
  }

  Answer answer = check_fact(relationType, _pImpl->_p.IsA, _pImpl->_p.RelationTypeCategory);

  if (answer.state == Answer::State::Known && answer.probability == 0)
  {
    throw std::runtime_error("add_fact: node is known to be no relation type");
  }

  answer = check_fact(source, relationType, target);

  if (answer.state == Answer::State::Known && answer.probability == 0)
  {
    throw std::runtime_error("add_fact: node is known to be no relation type");
  }
}
