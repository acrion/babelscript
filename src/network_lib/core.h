#pragma once

#include <cstdint>
#include <map>
#include <set>

namespace network
{
	template<typename A, typename B>
	class Core
	{
	public:
		void connect(A a, B b, long double probability=1)
		{
			_left[a].insert(b);
			_right[b].insert(a);

			if (probability < 1)
			{
				std::size_t hash = std::hash<A>()(a)^std::hash<B>()(b);
				typename decltype(_probabilities)::iterator it = _probabilities.find(hash);

				if (it == _probabilities.end())
				{
					_probabilities[hash] = probability;
				}
				else
				{
					it->second = std::min(it->second, probability);
				}
			}
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
		std::map<std::size_t, long double> _probabilities;
	};
}
