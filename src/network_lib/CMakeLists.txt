add_library (network_lib SHARED
  answer.cpp
  answer.h
  network_lib.cpp
  network_lib.h
  network_lib_impl.h
  network.h
  reasoning.cpp
  reasoning.h
  stopwatch.cpp
  stopwatch.h
  unification.cpp
  unification.h
  utils.cpp
  utils.h
)

generate_export_header(network_lib)
target_include_directories (network_lib PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE .)
include_directories(${Boost_INCLUDE_DIR})

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_target_properties(network_lib PROPERTIES MACOSX_RPATH ON)
endif()

if(NOT MSVC) # no auto linking available, so we have to set
  list(APPEND Boost_LIBRARIES ${BOOST_LIBRARYDIR}/libboost_system.a)
  list(APPEND Boost_LIBRARIES ${BOOST_LIBRARYDIR}/libboost_regex.a)
  list(APPEND Boost_LIBRARIES ${BOOST_LIBRARYDIR}/libboost_filesystem.a)
  list(APPEND Boost_LIBRARIES ${BOOST_LIBRARYDIR}/libboost_date_time.a)

  target_link_libraries(network_lib ${Boost_LIBRARIES})
  message(STATUS "Boost_LIBRARIES = '${Boost_LIBRARIES}'")
endif()
