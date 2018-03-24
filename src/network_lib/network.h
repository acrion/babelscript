#pragma once

#include <cstdint>
#include <map>
#include <set>

namespace network
{
  class Network
	{
	public:
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
        else
        {
          it->second = std::min(it->second, probability);
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
      Node hash;
      auto it = find_probability(a,b,hash);
      return it == _probabilities.end() ? 1:it->second;
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

    Node create_hash(Node a, Node b)
    {
      return std::hash<Node>()(a) ^ std::hash<Node>()(b);
    }

    Node create_hash(Node a, Node b, Node c)
    {
      return std::hash<Node>()(a) ^ std::hash<Node>()(b) ^ std::hash<Node>()(c);
    }

    Node create_hash(std::vector<Node> list)
    {
      Node result = Node();
      for (Node node: list) result ^= std::hash<Node>()(node);
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
