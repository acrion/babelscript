add_library (network_lib
	network_lib.cpp
	network_lib.h
	network_lib_impl.h
	core.h
)

include_directories(${Boost_INCLUDE_DIR})
target_include_directories (network_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

