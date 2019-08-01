add_library (babelscript SHARED
# babelscript::network
  answer.cpp
  answer.hpp
  babelscript.cpp
  babelscript.hpp
  babelscript_impl.hpp
  contradiction_error.hpp
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
# boost/serialization. We compile it ourselves, because using external lib boost in our 3x2 configs is hell. The configurations are (Mac, Linux, Windows) x (CMake, go). In addition we need to match the boost build with our build (e. g. regarding use of C++14).
  ../boost/serialization/src/archive_exception.cpp
  ../boost/serialization/src/basic_archive.cpp
  ../boost/serialization/src/basic_iarchive.cpp
  ../boost/serialization/src/basic_iserializer.cpp
  ../boost/serialization/src/basic_oarchive.cpp
  ../boost/serialization/src/basic_oserializer.cpp
  ../boost/serialization/src/basic_serializer_map.cpp
  ../boost/serialization/src/basic_text_iprimitive.cpp
  ../boost/serialization/src/basic_text_oprimitive.cpp
  ../boost/serialization/src/extended_type_info.cpp
  ../boost/serialization/src/extended_type_info_typeid.cpp
  ../boost/serialization/src/text_iarchive.cpp
  ../boost/serialization/src/text_oarchive.cpp
)

generate_export_header(babelscript)
target_include_directories (babelscript PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE .)
add_definitions(-DBOOST_ALL_NO_LIB) # disable boost auto linking under MSVC
include_directories(
  "../boost/algorithm/include"
  "../boost/archive/include"
  "../boost/assert/include"
  "../boost/bimap/include"
  "../boost/bind/include"
  "../boost/concept_check/include"
  "../boost/config_boost/include"
  "../boost/container_hash/include"
  "../boost/core/include"
  "../boost/detail/include"
  "../boost/foreach/include"
  "../boost/function/include"
  "../boost/integer/include"
  "../boost/io/include"
  "../boost/iterator/include"
  "../boost/move/include"
  "../boost/mpl/include"
  "../boost/multi_index/include"
  "../boost/predef/include"
  "../boost/preprocessor/include"
  "../boost/range/include"
  "../boost/serialization/include"
  "../boost/smart_ptr/include"
  "../boost/static_assert/include"
  "../boost/throw_exception/include"
  "../boost/tokenizer/include"
  "../boost/tuple/include"
  "../boost/type_index/include"
  "../boost/type_traits/include"
  "../boost/utility/include"
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_target_properties(babelscript PROPERTIES MACOSX_RPATH ON)
endif()
