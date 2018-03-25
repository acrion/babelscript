cmake_minimum_required (VERSION 2.8.11)
set(CMAKE_CXX_STANDARD 14)
project (network)
include(GenerateExportHeader)

#set(BOOST_BASE C:/T/Ophthalmic/Surgery/Branches/Development/Build/External)
#set(BOOST_VER 1.66.0.0)

#link_directories(             ${BOOST_BASE}/boost_filesystem-vc140.${BOOST_VER}/lib/native) 
#link_directories(             ${BOOST_BASE}/boost_system-vc140.${BOOST_VER}/lib/native)
#list(APPEND CMAKE_PREFIX_PATH ${BOOST_BASE}/boost.${BOOST_VER}/lib/native/include)

#find_package( boost REQUIRED)

# stuff for boost that must be done prior creating the target (because of link_directories)

# set compiler flags
if (WIN32)
  if (MSVC)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
      else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
      endif()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4275")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -Wall -pedantic")
    message(STATUS "${CMAKE_CXX_FLAGS}")
  endif()
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-D_DEBUG)
endif()

add_subdirectory (network_lib)
add_subdirectory (network_app)
