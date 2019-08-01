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
  ../boost/libs/serialization/src/archive_exception.cpp
  ../boost/libs/serialization/src/basic_archive.cpp
  ../boost/libs/serialization/src/basic_iarchive.cpp
  ../boost/libs/serialization/src/basic_iserializer.cpp
  ../boost/libs/serialization/src/basic_oarchive.cpp
  ../boost/libs/serialization/src/basic_oserializer.cpp
  ../boost/libs/serialization/src/basic_serializer_map.cpp
  ../boost/libs/serialization/src/basic_text_iprimitive.cpp
  ../boost/libs/serialization/src/basic_text_oprimitive.cpp
  ../boost/libs/serialization/src/extended_type_info.cpp
  ../boost/libs/serialization/src/extended_type_info_typeid.cpp
  ../boost/libs/serialization/src/text_iarchive.cpp
  ../boost/libs/serialization/src/text_oarchive.cpp
)

generate_export_header(babelscript)
target_include_directories (babelscript PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE .)
add_definitions(-DBOOST_ALL_NO_LIB) # disable boost auto linking under MSVC
include_directories(
  "../boost/libs/algorithm/include"
  "../boost/libs/archive/include"
  "../boost/libs/assert/include"
  "../boost/libs/bimap/include"
  "../boost/libs/bind/include"
  "../boost/libs/concept_check/include"
  "../boost/libs/config/include"
  "../boost/libs/container_hash/include"
  "../boost/libs/core/include"
  "../boost/libs/detail/include"
  "../boost/libs/foreach/include"
  "../boost/libs/function/include"
  "../boost/libs/integer/include"
  "../boost/libs/io/include"
  "../boost/libs/iterator/include"
  "../boost/libs/move/include"
  "../boost/libs/mpl/include"
  "../boost/libs/multi_index/include"
  "../boost/libs/predef/include"
  "../boost/libs/preprocessor/include"
  "../boost/libs/range/include"
  "../boost/libs/serialization/include"
  "../boost/libs/smart_ptr/include"
  "../boost/libs/static_assert/include"
  "../boost/libs/throw_exception/include"
  "../boost/libs/tokenizer/include"
  "../boost/libs/tuple/include"
  "../boost/libs/type_index/include"
  "../boost/libs/type_traits/include"
  "../boost/libs/utility/include"
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_target_properties(babelscript PROPERTIES MACOSX_RPATH ON)
endif()
