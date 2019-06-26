add_library (babelscript SHARED
# babelscript::network
  answer.cpp
  answer.hpp
  babelscript.cpp
  babelscript.hpp
  babelscript_impl.hpp
  network.hpp
  reasoning.cpp
  reasoning.hpp
  stopwatch.cpp
  stopwatch.hpp
  unification.cpp
  unification.hpp
  utils.cpp
  utils.hpp
# babelscript::console
  interactive.cpp
  interactive.hpp
  read_async.cpp
  read_async.hpp
  wikidata.cpp
  wikidata.hpp
)

generate_export_header(babelscript)
target_include_directories (babelscript PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE .)
include_directories(${Boost_INCLUDE_DIR})

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_target_properties(babelscript PROPERTIES MACOSX_RPATH ON)
endif()

if(NOT MSVC) # no auto linking available, so we have to set
  list(APPEND Boost_LIBRARIES ${BOOST_LIBRARYDIR}/libboost_system.a)
  list(APPEND Boost_LIBRARIES ${BOOST_LIBRARYDIR}/libboost_regex.a)
  list(APPEND Boost_LIBRARIES ${BOOST_LIBRARYDIR}/libboost_date_time.a)
  list(APPEND Boost_LIBRARIES ${BOOST_LIBRARYDIR}/libboost_serialization.a)

  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(babelscript ${Boost_LIBRARIES} stdc++fs) # stdc++fs must come after the source file in the command line, so we cannot add -lstdc++fs to CMAKE_CXX_FLAGS
  else()
    target_link_libraries(babelscript ${Boost_LIBRARIES})
  endif()

  message(STATUS "Boost_LIBRARIES = '${Boost_LIBRARIES}'")
endif()
