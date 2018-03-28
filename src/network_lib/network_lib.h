#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <set>
#include <functional>
#include "network_lib_export.h"
#include "answer.h"

namespace network
{
  class NETWORK_LIB_EXPORT Network_lib
	{    
  public:
		Network_lib();
		~Network_lib();

    Node node(std::wstring name, std::string lang);
    Node var();
    std::wstring get_name(Node node);
    std::wstring get_name(Node node, std::string lang);
    std::string get_name_hex(Node node, bool prepend_num=true);
    bool set_name(Node node, std::wstring name, std::string lang="en");
    std::set<Node> get_sources(Node relationType, Node target, bool exclude_vars=false);
    Node parse_fact(Node rule, std::set<Node>& deducations);
    std::set<Node> filter(std::set<Node> source, Node target);
    std::set<Node> filter(Node fact, Node relationType, Node target);
    std::set<Node> filter(std::set<Node> source, std::function<bool(Node nd)> f);
    Answer check_fact(Node source, Node relationType, const std::set<Node>& targets);
    Node fact(Node source, Node relationType, std::set<Node> targets, long double probability=1);
    Node condition(Node op, std::set<Node> conditions);
    void gen_dot(Node start, std::string file_name);

		void debug();
#ifdef _DEBUG
		template<typename MapType>
		void print_map(const MapType& m, std::wstring strArrow);
#endif

    // member list
    class Impl;
    Impl* const _pImpl; // must stay at top of members list because of initialization order

    const struct PredefinedNode
    {
      Node RelationTypeCategory;
      Node Causes;
      Node And;
      Node IsA;
      Node Unequal;
      Node Contradiction;
    } core;

  private:
    void add_nodes(Node current, std::set<Node>& touched, const std::set<Node>& conditions, const std::set<Node>& deductions, std::ofstream& dot);
  };
}
