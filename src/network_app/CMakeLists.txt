if (MSVC)
  add_definitions(-DBOOST_LIB_DIAGNOSTIC)
endif()

add_executable (network_app main.cpp interactive.cpp interactive.h)

target_link_libraries (network_app PRIVATE network_lib)
