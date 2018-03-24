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

void Network_lib::add_fact(Node source, Node relationType, Node target)
{
  if (source==target)
  {
    throw std::runtime_error("add_fact: source is identical to target");
  }

  //if ()
}
