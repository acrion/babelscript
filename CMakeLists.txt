cmake_minimum_required (VERSION 2.8.11)
project (network)

set(BOOST_BASE C:/T/Ophthalmic/Surgery/Branches/Development/Build/External)
set(BOOST_VER 1.66.0.0)

link_directories(             ${BOOST_BASE}/boost_filesystem-vc140.${BOOST_VER}/lib/native) 
link_directories(             ${BOOST_BASE}/boost_system-vc140.${BOOST_VER}/lib/native)
list(APPEND CMAKE_PREFIX_PATH ${BOOST_BASE}/boost.${BOOST_VER}/lib/native/include)

find_package( boost REQUIRED)

add_subdirectory (network_lib)
add_subdirectory (network_app)
