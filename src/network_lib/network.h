#pragma once

#include <cstdint>
#include <map>
#include <set>

namespace network
{
	template<typename A, typename B>
  class Network
	{
	public:
    void connect(A a, B b)
		{
			_left[a].insert(b);
			_right[b].insert(a);

		}

		typename std::map<A, std::set<B>>::iterator find_left(A a)
		{
			return _left.find(a);
		}

		typename std::map<A, std::set<B>>::iterator left_end()
		{
			return _left.end();
		}

		typename std::map<B, std::set<A>>::iterator find_right(B b)
		{
			return _right.find(b);
		}

		typename std::map<B, std::set<A>>::iterator right_end()
		{
			return _right.end();
		}

		bool find(A a, A& result, bool* isLeft=nullptr)
		{
			//static_assert(A == B);
			
			auto it = find_left(a);

			if (it == _left.end())
			{
				auto it2 = find_right(a);

				if (it2 == _right.end())
				{
					return false;
				}
				else if (isLeft)
				{
					*isLeft = false;
					return result = it2->second;
				}
			}
			else
			{
				if (isLeft)
				{
					*isLeft = true;
				}
				
				result = it->second;
			}

			return true;
		}

#ifndef _DEBUG
	private:
#endif
		std::map<A, std::set<B>> _left;
		std::map<B, std::set<A>> _right;
	};
}
