#pragma once

#include <cstdint>
#include <string>
#include "answer.h"

namespace network
{
  using Node = size_t;

  class Network_lib
	{
	public:
		Network_lib();
		~Network_lib();

    Node get_node(std::string lang, std::wstring name);
    std::wstring get_name(Node node, std::string lang="en");
    bool set_name(Node node, std::wstring name, std::string lang="en");
    void connect(Node a, Node b);
    Answer check_fact(Node source, Node relationType, Node target);
    void add_fact(Node source, Node relationType, Node target, long double probability=1);
		
		void debug();
#ifdef _DEBUG
		template<typename MapType>
		void print_map(const MapType& m);
#endif

	private:
		class Impl;
		Impl* _pImpl{ nullptr };
	};
}
