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
			std::wcout << getName(iter->first) << L" -->";

			for (int64_t a : iter->second)
			{
				std::wcout << L" " << getName(a);
			}

			std::wcout << std::endl;
		}
	}

	void Network_lib::debug()
	{
		std::cout << "left" << std::endl;
    print_map(_pImpl->_network._left);

		std::cout << "right" << std::endl;
    print_map(_pImpl->_network._right);
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

int64_t Network_lib::getNode(std::string lang, std::wstring name)
{
	decltype(_pImpl->_names)::iterator namesInLang = _pImpl->_names.find(lang);
	decltype(namesInLang->second.right)::iterator node;

	if (   namesInLang == _pImpl->_names.end()
	    || (node = namesInLang->second.right.find(name)) == namesInLang->second.right.end()
	   )
	{
		_pImpl->_names[lang].insert(Impl::Names(++_pImpl->_lastNode, name));
		return _pImpl->_lastNode;
	}
	else
	{
		return node->second;
	}
}

std::wstring Network_lib::getName(int64_t node, std::string lang)
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


bool Network_lib::setName(int64_t node, std::wstring newName, std::string lang)
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


void Network_lib::connect(int64_t a, int64_t b)
{
  _pImpl->_network.connect(a,b);
}
