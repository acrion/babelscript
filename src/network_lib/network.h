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
    const T mask = (CHAR_BIT * sizeof(n) - 1);
    c &= mask;
    return (n << c) | (n >> ((-c)&mask));
    }

    template<typename T>
    static inline T ror(T n, T c = 1)
    {
      const T mask = (CHAR_BIT * sizeof(n) - 1);
      c &= mask;
      return (n >> c) | (n << ((-c)&mask));
    }
#ifdef _MSC_VER
#pragma warning( pop )
#endif

    void connect(Node a, Node b, long double probability = 1)
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
        auto it = find_probability(a, b, hash);

        if (it == _probabilities.end())
        {
          _probabilities[hash] = probability;
        }
        else if (it->second >= 0.5L && probability >= 0.5L)
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
      auto it = _left.find(a); if (it != _left.end() && it->second.count(b) == 1) return 1; // precedence over reverse (-1)
      it = _left.find(b); if (it != _left.end() && it->second.count(a) == 1) return -1;

      return 0;
    }

    long double probability(Node a, Node b)
    {
      if (is_var(a) || is_var(b))
      {
        return 1;
      }
      
      auto itLeft = _left.find(a);
      if (itLeft != _left.end() && itLeft->second.count(b) == 1)
      {
        Node hash;
        auto it = find_probability(a, b, hash);
        return it == _probabilities.end() ? 1 : it->second;
      }
      else
      {
        return 0;
      }
    }

    Node create()
    {
      while (_left.find(++_last) != _left.end())
        ;

      if (is_var(_last))
      {
        throw std::logic_error("Network::var: Exceeded maximum number of " + std::to_string(_last-1) + " nodes.");
      }

      _left[_last] = std::set<Node>();
      _right[_last] = std::set<Node>();
      return _last;
    }

    Node var()
    {
      if (_left.find(--_last_var) != _left.end())
      {
        throw std::runtime_error("Network::var: Node " + std::to_string(_last_var) + " already in use");
      }

      if (!is_var(_last_var))
      {
        throw std::logic_error("Network::var: Exceeded maximum number of " + std::to_string(std::numeric_limits<Node>::max() - _last_var) + " variables. Please note that variables may be reused in separate rules.");
      }

      _left[_last_var] = std::set<Node>();
      _right[_last_var] = std::set<Node>();

      return _last_var;
    }

    bool is_var(Node a)
    {
      return a > mask_node;
    }

    void create(Node a)
    {
      if (_left.find(a) != _left.end())
      {
        throw std::runtime_error("Network::create: requested node " + std::to_string(a) + " already in use");
      }

      if (is_var(a))
      {
        throw std::runtime_error("Network::create: requested node " + std::to_string(a) + " conflicts with variable values");
      }

      _left[a] = std::set<Node>();
      _right[a] = std::set<Node>();
    }

    Node create_hash(Node a, Node b)
    {
      return mask_node & (mark_hash ^ a ^ rol(b,shift_inc));
    }

    Node create_hash(Node a, Node b, Node c)
    {
      return mask_node & (mark_hash ^ a ^ rol(b,shift_inc) ^ rol(c,2*shift_inc));
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
      return result & mask_node; // exclude values that denote variables
    }

    typename std::map<Node, std::set<Node>>::iterator find_left(Node a)  { return  _left.find(a); }
    typename std::map<Node, std::set<Node>>::iterator find_right(Node b) { return _right.find(b); }
    typename std::map<Node, std::set<Node>>::iterator  left_end()     { return  _left.end();	 }
    typename std::map<Node, std::set<Node>>::iterator right_end()     { return _right.end();   }

#ifndef _DEBUG
	private:
#endif
    static constexpr Node shift_inc = 5;
    static constexpr Node mark_hash = 0x4000000000000000ul;
    static constexpr Node mask_node = 0x7FFFFFFFFFFFFFFFul;

    std::map<Node, std::set<Node>> _left;
    std::map<Node, std::set<Node>> _right;
    std::map<Node, long double> _probabilities;
    Node _last{ Node() };
    Node _last_var{ Node() };

    typename decltype(_probabilities)::iterator find_probability(Node a, Node b, Node& hash)
    {
      hash = create_hash(a, b);
      return _probabilities.find(hash);
    }
  };
}
