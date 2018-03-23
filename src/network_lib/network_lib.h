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

		int64_t getNode(std::string lang, std::wstring name);
		std::wstring getName(int64_t node, std::string lang="en");
    bool setName(int64_t node, std::wstring name, std::string lang="en");
		void connect(int64_t a, int64_t b);
		double getProbability(int64_t a, int64_t b);
		
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
