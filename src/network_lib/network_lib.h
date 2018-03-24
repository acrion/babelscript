#pragma once

#include <cstdint>
#include <string>

namespace network
{
	class Network_lib
	{
	public:
		Network_lib();
		~Network_lib();

    uint64_t getNode(std::string lang, std::wstring name);
    std::wstring getName(uint64_t node, std::string lang="en");
    bool setName(uint64_t node, std::wstring name, std::string lang="en");
    void connect(uint64_t a, uint64_t b);
		
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
