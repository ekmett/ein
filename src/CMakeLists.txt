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

add_library(doctest_includes INTERFACE)
target_include_directories(doctest_includes INTERFACE "${FETCHCONTENT_BASE_DIR}/doctest-src/doctest")
list(APPEND CMAKE_MODULE_PATH "${FETCHCONTENT_BASE_DIR}/doctest-src/scripts/cmake")
include(doctest)

# header jiujutsu
# ====================================================================

set(EIN_SOURCE_HEADER_FILES
  ein/attributes/calling_conventions.hpp
  ein/attributes/common.hpp
  ein/attributes/control_flow.hpp
  ein/attributes/cuda.hpp
  ein/attributes/detection.hpp
  ein/attributes/handles.hpp
  ein/attributes/initialization.hpp
  ein/attributes/inlining.hpp
  ein/attributes/lifetimes.hpp
  ein/attributes/linkage.hpp
  ein/attributes/memory.hpp
  ein/attributes/null_safety.hpp
  ein/attributes/performance_constraints.hpp
  ein/attributes/purity.hpp
  ein/attributes/strings.hpp
  ein/attributes/targets.hpp
  ein/attributes/thread_safety.hpp
  ein/attributes/typestate.hpp
  ein/assert.hpp
  ein/concepts.hpp
  ein/cpuid.hpp
  ein/memory.hpp
  ein/numerics/bf16.hpp
  ein/numerics/fp16.hpp
  ein/numerics.hpp
  ein/profiling.hpp
#  ein/simd.hpp
#  ein/simd_data.hpp
  ein/static_string.hpp
  ein/types.hpp
  ein/wait.hpp
)

set(EIN_SOURCE_FILES
  ein/cpuid.cpp
  ein/numerics/bf16.cpp
  ein/numerics/fp16.cpp
  ein/numerics.cpp
  ein/static_string.cpp
  ein/wait.cpp
)

add_library(ein STATIC ${EIN_SOURCE_FILES})
target_sources(ein
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ein" "${CMAKE_CURRENT_BINARY_DIR}/../include"
    FILES ${EIN_SOURCE_HEADER_FILES} ${CMAKE_CURRENT_BINARY_DIR}/../include/ein/config.hpp
)

target_link_libraries(ein
  PUBLIC
    compiler_rt_builtins
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    Threads::Threads
)

target_include_directories(ein PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/ein>)

install(TARGETS ein
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  INCLUDES DESTINATION include)

set_target_properties(ein PROPERTIES PUBLIC_HEADER "${EIN_SOURCE_HEADER_FILES};${CMAKE_CURRENT_BINARY_DIR}/../include/ein/config.hpp")
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-std=c++23>") # why isn't this being set?!
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-msse4.2;-mavx;-mbmi2;-mfma;-mavx2;-ftree-vectorize;-funroll-loops;-mwaitpkg;-mmwaitx>")
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-mavx512f;-mavx512dq;-mavx512bw;-mavx512vl;-mavx512vbmi2;-mavx512vp2intersect;-mavx512bf16>")
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-Wno-unqualified-std-cast-call>")

add_executable(ein_tests ein/tests.cpp)
target_link_libraries(ein_tests PRIVATE ein doctest_includes)
# add_test(NAME doctest COMMAND doctest_ein)
doctest_discover_tests(ein_tests)
