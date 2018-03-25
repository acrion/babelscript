#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "network_lib_export.h"
#include "answer.h"

namespace network
{
  class NETWORK_LIB_EXPORT Network_lib
	{    
  private:
    class Impl;
    Impl* _pImpl{ nullptr };

  public:
		Network_lib();
		~Network_lib();

    Node node(std::wstring name, std::string lang);
    Node var();
    std::wstring get_name(Node node);
    std::wstring get_name(Node node, std::string lang);
    bool set_name(Node node, std::wstring name, std::string lang="en");
    void connect(Node a, Node b);
    Answer check_fact(Node source, Node relationType, Node target);
    Node fact(Node source, Node relationType, Node target, long double probability=1);
    Node fact(std::vector<Node> conditions, Node relation, Node deduction, long double probability=1);
    void run();
		
		void debug();
#ifdef _DEBUG
		template<typename MapType>
		void print_map(const MapType& m, std::wstring strArrow);
#endif

    const struct PredefinedNode
    {
      Node RelationTypeCategory;
      Node Rule;
      Node And;
      Node IsA;
      Node Unequal;
      Node Contradiction;
    } core;
	};
}
