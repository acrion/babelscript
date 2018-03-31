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

      if (probability < 1)
      {
        if (is_var(a | b))
        {
          throw std::runtime_error("Network::connect: setting probabilities for connection that include variables");
        }

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

      leftIt->second.insert(b);
      rightIt->second.insert(a);
    }

    bool exists(Node a)
    {
      return _left.find(a) != _left.end();
    }

    //int is_connected(Node a, Node b)
    //{
    //  auto it = _left.find(a); if (it != _left.end() && it->second.count(b) == 1) return 1; // precedence over reverse (-1)
    //  it = _left.find(b); if (it != _left.end() && it->second.count(a) == 1) return -1;

    //  return 0;
    //}

    long double probability(Node a, Node b)
    {
      if (is_var(a|b))
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
        throw std::logic_error("Network::var: Exceeded maximum number of " + std::to_string(std::numeric_limits<Node>::max() - _last_var) + " variables.");
      }

       _left[_last_var] = std::set<Node>();
      _right[_last_var] = std::set<Node>();

      return _last_var;
    }

    bool is_var(Node a)
    {
      return a > mask_node;
    }

    bool is_hash(Node a)
    {
      return (a & mark_hash) == mark_hash;
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
      return mask_node & (mark_hash ^ mod(a) ^ rol(mod(b),shift_inc));
    }

    Node create_hash(Node a, Node b, Node c)
    {
      return mask_node & (mark_hash ^ mod(a) ^ rol(mod(b),shift_inc) ^ rol(mod(c),2*shift_inc));
    }

    Node create_hash(const std::set<Node>& vec)
    {
      Node result = mark_hash ^ rol(vec.size(), 56);
      for (Node node: vec)
      {
        result ^= mod(rol(node, 48));
      }
      return result & mask_node; // exclude values that denote variables
    }

    Node create_hash(const Node head, const std::set<Node>& vec)
    {
      return (  create_hash(vec)
              ^ rol(mod(head), 32)
             ) & mask_node;
    }

    Node create_hash(const Node head1, const Node head2, const std::set<Node>& vec)
    {
      return (  create_hash(vec)
              ^ rol(mod(head1), 32)
              ^ rol(mod(head2), 40)
             ) & mask_node;
    }

    const std::map<Node, std::set<Node>> all_left()
    {
      return _left;
    }

    const std::map<Node, std::set<Node>> all_right()
    {
      return _right;
    }

    typename std::map<Node, std::set<Node>>::iterator find_right(Node a) { return  _left.find(a); }
    typename std::map<Node, std::set<Node>>::iterator find_left(Node b)  { return _right.find(b); }
    typename std::map<Node, std::set<Node>>::iterator  left_end()     { return  _left.end();	 }
    typename std::map<Node, std::set<Node>>::iterator right_end()     { return _right.end();   }

    const std::set<Node>& get_left(Node b)  { auto it = _right.find(b); return it == _right.end() ? _empty : it->second; }
    const std::set<Node>& get_right(Node b) { auto it = _left.find(b);  return it == _left.end() ? _empty : it->second;  }

#ifndef _DEBUG
	private:
#endif
    static constexpr Node shift_inc = 5;
    static constexpr Node mark_hash           = 0x4000000000000000ul;
    static constexpr Node mask_node           = 0x7FFFFFFFFFFFFFFFul; // mask highest bit
    static constexpr Node mask_highest_2_bits = 0x3ffffffffffffffful;

    std::map<Node, std::set<Node>> _left;
    std::map<Node, std::set<Node>> _right;
    std::map<Node, long double> _probabilities;
    const std::set<Node> _empty;
    Node _last{ Node() };
    Node _last_var{ Node() };

    typename decltype(_probabilities)::iterator find_probability(Node a, Node b, Node& hash)
    {
      hash = create_hash(a, b);
      return _probabilities.find(hash);
    }

#ifdef _MSC_VER
    #pragma warning( push )  
    #pragma warning( disable : 4146 )
#endif
    static inline uint64_t rol(uint64_t n, uint64_t c = 1)
    {
      const uint64_t mask = 8*sizeof(n)-1;
      c &= mask;
      //uint64_t wrapped = n >> ((-c)&mask);
      uint64_t wrapped = (n & mask_highest_2_bits) >> (((-c)&mask) - 2); // we want to wrap already at bit 61 (not 63), since mask_node clears bit 63 and we use bit 62 to denote hashes (mark_hash)
      return ((n << c) | wrapped) & mask_highest_2_bits;
    }

    static inline uint64_t ror(uint64_t n, uint64_t c = 1)
    {
      const uint64_t mask = 8*sizeof(n)-1;
      c &= mask;
      return (n >> c) | (n << ((-c)&mask));
    }
#ifdef _MSC_VER
    #pragma warning( pop )
#endif

    static uint64_t mod(uint64_t n)
    {
      // We generate nodes both by counting counting up and down (for vars) from 0, which increases probability of hash collisions.
      // So, make a clear different between those two categories.
      return n > mask_node ? ror(n, 32) : n;
    }
  };
}
