add_library (network_lib SHARED
  answer.cpp
  answer.h
  network_lib.cpp
  network_lib.h
  network_lib_impl.h
  network.h
  reasoning.cpp
  reasoning.h
  unification.cpp
  unification.h
  utils.cpp
  utils.h
)

generate_export_header(network_lib)
target_include_directories (network_lib PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE .)
include_directories(${Boost_INCLUDE_DIR})


if(NOT MSVC) # no auto linking available, so we have to set
  list(APPEND Boost_LIBRARIES ${BOOST_LIBRARYDIR}/libboost_system.a)
  list(APPEND Boost_LIBRARIES ${BOOST_LIBRARYDIR}/libboost_regex.a)
  list(APPEND Boost_LIBRARIES ${BOOST_LIBRARYDIR}/libboost_filesystem.a)
  list(APPEND Boost_LIBRARIES ${BOOST_LIBRARYDIR}/libboost_date_time.a)

  if(WIN32)
      # append suffixes for MinGW
      set(BOOST_VERSION "1_67")
      if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
          set(BOOST_LIB_SUFFIX "-mgw53-mt-d-x32-${BOOST_VERSION}.a")
      else()
          set(BOOST_LIB_SUFFIX "-mgw53-mt-x32-${BOOST_VERSION}.a")
      endif()

      message(STATUS "Making sure boost libraries have suffix '${BOOST_LIB_SUFFIX}'")

      foreach(sharedLib ${Boost_LIBRARIES})
          message(STATUS "orig:  ${sharedLib}")
          string(REPLACE ".a" "${BOOST_LIB_SUFFIX}" sharedLib "${sharedLib}")
          message(STATUS "fixed: ${sharedLib}")
          list(APPEND Fixed_Boost_LIBRARIES "${sharedLib}")
      endforeach()
  else()
      set(Fixed_Boost_LIBRARIES "${Boost_LIBRARIES}")
  endif()
  target_link_libraries(network_lib ${Fixed_Boost_LIBRARIES})
  message(STATUS "Boost_LIBRARIES = '${Fixed_Boost_LIBRARIES}'")
endif()
