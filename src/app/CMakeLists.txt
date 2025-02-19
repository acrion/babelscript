add_executable (babelscript_app main.cpp)

message(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  add_definitions(-DBOOST_LIB_DIAGNOSTIC)
  target_link_libraries (babelscript_app PRIVATE babelscript)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
  target_link_libraries(babelscript_app PRIVATE Threads::Threads babelscript stdc++fs)
else ()
  target_link_libraries (babelscript_app PRIVATE babelscript)
endif()
