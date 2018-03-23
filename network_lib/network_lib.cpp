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
		print_map(_pImpl->_core._left);

		std::cout << "right" << std::endl;
		print_map(_pImpl->_core._right);
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


void Network_lib::connect(int64_t a, int64_t b)
{
	_pImpl->_core.connect(a,b);
}


double Network_lib::getProbability(int64_t a, int64_t b)
{
//	auto it = _pImpl->_data.left.find(a);

	//if (it == _data.end())
	//{
	//	return 0;
	//}

	//auto it2 = it->get_right()

	return 0;
}

