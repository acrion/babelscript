#pragma once

#include <cstdint>
#include <map>
#include <set>

namespace network
{
  template<typename T>
  class Network
	{
	public:
    void connect(T a, T b)
		{
      auto leftIt = _left.find(a);
      auto rightIt = _right.find(b);

      if (leftIt == _left.end())
      {
        throw std::runtime_error("Network::connect: requested left node " + std::to_string(a) + " does not exist");
      }

      if (rightIt == _right.end())
      {
        throw std::runtime_error("Network::connect: requested right node " + std::to_string(b) + " does not exist");
      }

      leftIt->second.insert(b);
      rightIt->second.insert(a);
		}

    T create()
    {
      while(_left.find(++_last) != _left.end());

      _left[_last] = std::set<T>();
      _right[_last] = std::set<T>();
      return _last;
    }

    void create(T a)
    {
      if (_left.find(a) != _left.end())
      {
        throw std::runtime_error("Network::create: requested node " + std::to_string(a) + " already in use");
      }

      _left[a] = std::set<T>();
      _right[a] = std::set<T>();
    }

    typename std::map<T, std::set<T>>::iterator find_left(T a)  { return  _left.find(a); }
    typename std::map<T, std::set<T>>::iterator left_end()      { return  _left.end();	 }
    typename std::map<T, std::set<T>>::iterator find_right(T b) { return _right.find(b); }
    typename std::map<T, std::set<T>>::iterator right_end()     { return _right.end();   }

#ifndef _DEBUG
	private:
#endif
    std::map<T, std::set<T>> _left;
    std::map<T, std::set<T>> _right;
    T _last{T()};
	};
}
