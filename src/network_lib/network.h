#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <vector>
#include <limits.h>

namespace network
{
  class Network
  {
  public:
#ifdef _MSC_VER
    #pragma warning( push )  
    #pragma warning( disable : 4146 )
#endif
    template<typename T>
    static inline T rol(T n, T c=1)
    {
      const T mask = (CHAR_BIT*sizeof(n) - 1);
      c &= mask;
      return (n<<c) | (n >> ((-c)&mask));
    }

    template<typename T>
    static inline T ror(T n, T c=1)
    {
      const T mask = (CHAR_BIT*sizeof(n) - 1);
      c &= mask;
      return (n>>c) | (n << ((-c)&mask));
    }
#ifdef _MSC_VER
    #pragma warning( pop )
#endif

    void connect(Node a, Node b, long double probability=1)
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

      if (probability < 1)
      {
        Node hash;
        auto it = find_probability(a,b,hash);

        if (it == _probabilities.end())
        {
          _probabilities[hash] = probability;
        }
        else if (it->second>=0.5L && probability>=0.5L)
        {
          it->second = std::max(it->second, probability);
        }
        else if (it->second <= 0.5L && probability <= 0.5L)
        {
          it->second = std::min(it->second, probability);
        }
        else
        {
          throw std::runtime_error("Network::connect: nodes have contradicting probabilities");
        }
      }
    }

    bool exists(Node a)
    {
      return _left.find(a) != _left.end();
    }

    int is_connected(Node a, Node b)
    {
      auto it = _left.find(a); if (it != _left.end() && it->second.count(b)==1) return 1;
           it = _left.find(b); if (it != _left.end() && it->second.count(a)==1) return -1;

      return 0;
    }

    long double probability(Node a, Node b)
    {
      int sgn = is_connected(a,b);
      if (sgn)
      {
        Node hash;
        auto it = find_probability(a,b,hash);
        return it == _probabilities.end() ? sgn:sgn*it->second;
      }
      else
      {
        return 0;
      }
    }

    Node create()
    {
      while(_left.find(++_last) != _left.end())
        ;

       _left[_last] = std::set<Node>();
      _right[_last] = std::set<Node>();
      return _last;
    }

    void create(Node a)
    {
      if (_left.find(a) != _left.end())
      {
        throw std::runtime_error("Network::create: requested node " + std::to_string(a) + " already in use");
      }

      _left[a] = std::set<Node>();
      _right[a] = std::set<Node>();
    }

    static constexpr Node mark_hash = 0x4000000000000000ul;
    static constexpr Node shift_inc = 5;

    Node create_hash(Node a, Node b)
    {
      return mark_hash ^ a ^ rol(b,shift_inc);
    }

    Node create_hash(Node a, Node b, Node c)
    {
      return mark_hash ^ a ^ rol(b,shift_inc) ^ rol(c,2*shift_inc);
    }

    Node create_hash(const std::vector<Node>& vec)
    {
      Node result = mark_hash;
      Node shift=1;
      for (Node node: vec)
      {
        result ^= rol(node, shift);
        shift = (shift+shift_inc) % sizeof(Node);
      }
      return result;
    }

    typename std::map<Node, std::set<Node>>::iterator find_left(Node a)  { return  _left.find(a); }
    typename std::map<Node, std::set<Node>>::iterator find_right(Node b) { return _right.find(b); }
    typename std::map<Node, std::set<Node>>::iterator  left_end()     { return  _left.end();	 }
    typename std::map<Node, std::set<Node>>::iterator right_end()     { return _right.end();   }

#ifndef _DEBUG
	private:
#endif
    std::map<Node, std::set<Node>> _left;
    std::map<Node, std::set<Node>> _right;
    std::map<Node, long double> _probabilities;
    Node _last{Node()};

    typename decltype(_probabilities)::iterator find_probability(Node a, Node b, Node& hash)
    {
      return _probabilities.find(hash=create_hash(a,b));
    }
  };
}
