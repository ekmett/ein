# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

find_package(Threads REQUIRED)

# nlohmann's json library provides a solid c++ API for working with json
fetchcontentcached(
  NAME json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.3
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)

FetchContentCached(
  NAME doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  GIT_TAG v2.4.11
  SOURCE_SUBDIR doctest
)

# set(CATCH_INSTALL_DOCS OFF CACHE BOOL "skip catch docs")

# FetchContentCached(
#   NAME Catch2
#   GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#   GIT_TAG        v3.7.1 # or a later release
# )
#FetchContent_MakeAvailable(Catch2)

# silence a third-party warning on clang++-18+
# target_compile_options(Catch2 PRIVATE
#   $<$<AND:$<CXX_COMPILER_ID:Clang>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,18>>:-Wno-unused-but-set-variable>
# )

# list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")

#include(Catch)

#add_executable(t_ein bf16.cpp)
#target_link_libraries(t_ein PRIVATE Catch2::Catch2WithMain ein)
# catch_discover_tests(t_ein)


# header jiujutsu
# ====================================================================


file(GLOB_RECURSE SOURCE_HEADER_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ein/*.hpp")
add_custom_target(ein_headers ALL DEPENDS ${SOURCE_HEADER_FILES} ${CMAKE_CURRENT_BINARY_DIR}/../include/ein/config.hpp)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ein/*.cpp")
add_library(ein STATIC ${SOURCE_FILES} ${SOURCE_HEADER_FILES})

#  numerics.cpp
#  pretty.cpp

add_dependencies(ein ein_headers)
add_compile_definitions(ENABLE_TESTS=1)

target_link_libraries(ein PUBLIC compiler_rt_builtins nlohmann_json::nlohmann_json spdlog::spdlog Threads::Threads)

target_include_directories(ein PRIVATE ${FETCHCONTENT_BASE_DIR}/doctest-src/doctest)
target_include_directories(ein PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/ein>)

install(TARGETS ein
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include/ein
  INCLUDES DESTINATION include/ein)

set_target_properties(ein PROPERTIES PUBLIC_HEADER "${OUTPUT_HEADER_FILES}")

target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-std=c++23>") # why isn't this being set?!
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-msse4.2;-mavx;-mbmi2;-mfma;-mavx2;-ftree-vectorize;-funroll-loops;-mwaitpkg;-mmwaitx>")
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-mavx512f;-mavx512dq;-mavx512bw;-mavx512vl;-mavx512vbmi2;-mavx512vp2intersect;-mavx512bf16>")
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-Wno-unqualified-std-cast-call>")

add_executable(doctest_ein doctest.cpp)
target_include_directories(doctest_ein PRIVATE ${FETCHCONTENT_BASE_DIR}/doctest-src/doctest)
target_link_libraries(doctest_ein PRIVATE ein)
add_test(NAME doctest COMMAND doctest_ein)
