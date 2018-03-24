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
			_left[a].insert(b);
			_right[b].insert(a);
		}

    typename std::map<T, std::set<T>>::iterator find_left(T a)  { return  _left.find(a); }
    typename std::map<T, std::set<T>>::iterator left_end()      { return  _left.end();	 }
    typename std::map<T, std::set<T>>::iterator find_right(T b) { return _right.find(b); }
    typename std::map<T, std::set<T>>::iterator right_end()     { return _right.end();   }

    int find(T a, T& result)
		{
			auto it = find_left(a);

      if (it != left_end())
      {
        result = it->second;
        return 1;
      }

      it = find_right(a);

      if (it != right_end())
      {
        result = it->second;
        return -1;
      }

      return 0;
		}

#ifndef _DEBUG
	private:
#endif
    std::map<T, std::set<T>> _left;
    std::map<T, std::set<T>> _right;
	};
}
