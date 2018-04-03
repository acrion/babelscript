#pragma once

#include <cstdint>
#include <map>
#include <unordered_set>
#include <vector>
#include <limits.h>
#include <mutex>

namespace network
{
  class Network
  {
  public:
    void connect(Node a, Node b, long double probability = 1)
    {
      std::lock_guard<std::mutex> lock(_mtx_left);
      std::lock_guard<std::mutex> lock2(_mtx_right);
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
        std::lock_guard<std::mutex> lock3(_mtx_prob);
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
      std::lock_guard<std::mutex> lock(_mtx_left);
      return _left.find(a) != _left.end();
    }

    long double probability(Node a, Node b)
    {
      if (is_var(a|b))
      {
        return 1;
      }
      
      std::lock_guard<std::mutex> lock(_mtx_left);
      auto itLeft = _left.find(a);
      if (itLeft != _left.end() && itLeft->second.count(b) == 1)
      {
        Node hash;
        std::lock_guard<std::mutex> lock3(_mtx_prob);
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
      std::lock_guard<std::mutex> lock(_mtx_left);
      std::lock_guard<std::mutex> lock2(_mtx_right);

      while (_left.find(++_last) != _left.end())
          ;

      if (is_var(_last))
      {
        throw std::logic_error("Network::var: Exceeded maximum number of " + std::to_string(_last-1) + " nodes.");
      }

      _left[_last] = std::unordered_set<Node>();
      _right[_last] = std::unordered_set<Node>();
      return _last;
    }

    Node var()
    {
      std::lock_guard<std::mutex> lock(_mtx_left);
      std::lock_guard<std::mutex> lock2(_mtx_right);

      if (_left.find(--_last_var) != _left.end())
      {
        throw std::runtime_error("Network::var: Node " + std::to_string(_last_var) + " already in use");
      }

      if (!is_var(_last_var))
      {
        throw std::logic_error("Network::var: Exceeded maximum number of " + std::to_string(std::numeric_limits<Node>::max() - _last_var) + " variables.");
      }

      _left[_last_var] = std::unordered_set<Node>();
      _right[_last_var] = std::unordered_set<Node>();

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
      std::lock_guard<std::mutex> lock(_mtx_left);
      std::lock_guard<std::mutex> lock2(_mtx_right);

      if (_left.find(a) != _left.end())
      {
        throw std::runtime_error("Network::create: requested node " + std::to_string(a) + " already in use");
      }

      if (is_var(a))
      {
        throw std::runtime_error("Network::create: requested node " + std::to_string(a) + " conflicts with variable values");
      }

      _left[a] = std::unordered_set<Node>();
      _right[a] = std::unordered_set<Node>();
    }

    Node create_hash(Node a, Node b)
    {
      return mask_node & (mark_hash ^ mod(a) ^ rol(mod(b),shift_inc));
    }

    Node create_hash(Node a, Node b, Node c)
    {
      return mask_node & (mark_hash ^ mod(a) ^ rol(mod(b),shift_inc) ^ rol(mod(c),2*shift_inc));
    }

    Node create_hash(const std::unordered_set<Node>& vec)
    {
      Node result = mark_hash ^ rol(vec.size(), 56);
      for (Node node: vec)
      {
        result ^= mod(rol(node, 48));
      }
      return result & mask_node; // exclude values that denote variables
    }

    Node create_hash(const Node head, const std::unordered_set<Node>& vec)
    {
      return (  create_hash(vec)
              ^ rol(mod(head), 32)
             ) & mask_node;
    }

    Node create_hash(const Node head1, const Node head2, const std::unordered_set<Node>& vec)
    {
      return (  create_hash(vec)
              ^ rol(mod(head1), 32)
              ^ rol(mod(head2), 40)
             ) & mask_node;
    }

    typename std::map<Node, std::unordered_set<Node>>::iterator find_right(Node a)
    {
      std::lock_guard<std::mutex> lock(_mtx_left);
      return  _left.find(a);
    }
    typename std::map<Node, std::unordered_set<Node>>::iterator find_left(Node b)
    {
      std::lock_guard<std::mutex> lock(_mtx_right);
      return _right.find(b);
    }
    typename std::map<Node, std::unordered_set<Node>>::iterator  left_end()
    {
      std::lock_guard<std::mutex> lock(_mtx_left);
      return  _left.end();
    }
    typename std::map<Node, std::unordered_set<Node>>::iterator right_end()
    {
      std::lock_guard<std::mutex> lock(_mtx_right);
      return _right.end();
    }

    const std::unordered_set<Node>& get_left(Node b)
    {
      std::lock_guard<std::mutex> lock(_mtx_left);
      auto it = _right.find(b); return it == _right.end() ? _empty : it->second;
    }
    const std::unordered_set<Node>& get_right(Node b)
    {
      std::lock_guard<std::mutex> lock(_mtx_right);
      auto it = _left.find(b);  return it == _left.end() ? _empty : it->second;
    }

#ifndef _DEBUG
	private:
#endif
    static constexpr Node shift_inc = 5;
    static constexpr Node mark_hash           = 0x4000000000000000ull;
    static constexpr Node mask_node           = 0x7FFFFFFFFFFFFFFFull; // mask highest bit
    static constexpr Node mask_highest_2_bits = 0x3fffffffffffffffull;

    std::map<Node, std::unordered_set<Node>> _left;
    std::map<Node, std::unordered_set<Node>> _right;
    std::map<Node, long double> _probabilities;
    const std::unordered_set<Node> _empty;
    Node _last{ Node() };
    Node _last_var{ Node() };
    std::mutex _mtx_prob;
    std::mutex _mtx_left;
    std::mutex _mtx_right;

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
      uint64_t wrapped = (n & mask_highest_2_bits) >> (((-c)&mask) - 2); // we want to wrap already at bit 61 (not 63), since mask_node clears bit 63 (which marks vars) and we use bit 62 to denote hashes (mark_hash)
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
      // We generate nodes both by counting up and down (for vars) from 0, which increases probability of hash collisions.
      // So, make a clear different between those two categories.
      return n > mask_node ? ror(n, 32) : n;
    }
  };
}
