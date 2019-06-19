cmake_minimum_required (VERSION 2.8.11)
set(CMAKE_CXX_STANDARD 17)
project (babelscript)
include(GenerateExportHeader)

set(BOOST_INCLUDEDIR "${CMAKE_BINARY_DIR}/../../boost")
set(BOOST_LIBRARYDIR ${BOOST_INCLUDEDIR}/stage/lib)
set( Boost_USE_STATIC_LIBS ON )
include_directories(
    ${BOOST_INCLUDEDIR}
    ${BOOST_INCLUDEDIR}/libs/dll/include
    ${BOOST_INCLUDEDIR}/libs/bimap/include
    ${BOOST_INCLUDEDIR}/libs/container_hash/include
    ${BOOST_INCLUDEDIR}/libs/core/include
)
if (MSVC) # with Visual Studio we can rely on boost auto-linking
  add_definitions(-DBOOST_LIB_DIAGNOSTIC)
  link_directories(${BOOST_LIBRARYDIR})
endif()

# set compiler flags
if (WIN32)
  message(STATUS "CMake platform: WIN32")
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
  message(STATUS "CMake platform: NOT WIN32")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-D_DEBUG)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory (lib)
add_subdirectory (app)
