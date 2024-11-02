# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

if (NOT CODE_COVERAGE)
  set(UNITY_BUILD_BATCH_SIZE 16)
endif()

if (NOT CODE_COVERAGE)
  set(UNITY_BUILD_BATCH_SIZE 4)
endif()


# header jiujutsu
# ====================================================================

set(EIN_SOURCE_HEADER_FILES
  ein/attributes/calling_conventions.hpp
  ein/attributes/common.hpp
  ein/attributes/control_flow.hpp
  ein/attributes/cuda.hpp
  ein/attributes/handles.hpp
  ein/attributes/memory.hpp
  ein/attributes/null_safety.hpp
  ein/attributes/performance_constraints.hpp
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

target_precompile_headers(ein
PRIVATE
  <bit>
  <compare>
  <cstdint>
  <limits>
  <string>
  <string_view>
  <type_traits>
  <cxxabi.h>
  <x86intrin.h>
  <spdlog/spdlog.h>
  <nlohmann/json.hpp>
  ein/attributes/common.hpp # only headers without tests unless we can't reuse it for ein_tests anyways
#  ein/types.hpp
#  ein/static_string.hpp
#  ein/concepts.hpp
)

set_target_properties(ein PROPERTIES PUBLIC_HEADER "${EIN_SOURCE_HEADER_FILES};${CMAKE_CURRENT_BINARY_DIR}/../include/ein/config.hpp")
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-std=c++23>") # why isn't this being set?!
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-msse4.2;-mavx;-mbmi2;-mfma;-mavx2;-ftree-vectorize;-funroll-loops;-mwaitpkg;-mmwaitx>")
if(AVX512)
  target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-mavx512f;-mavx512dq;-mavx512bw;-mavx512vl;-mavx512vbmi2;-mavx512vp2intersect;-mavx512bf16>")
endif()
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-Wno-unqualified-std-cast-call>")

add_executable(ein_tests ein/tests.cpp)

if(CODE_COVERAGE)
  target_compile_options(ein PRIVATE -fprofile-instr-generate -fcoverage-mapping)
  target_link_options(ein PRIVATE -fprofile-instr-generate -fcoverage-mapping)
  target_compile_options(ein_tests PRIVATE -fprofile-instr-generate -fcoverage-mapping)
  target_link_options(ein_tests PRIVATE -fprofile-instr-generate -fcoverage-mapping)
endif()

target_compile_options(ein_tests PUBLIC "$<${CXX_GccOrClang}:-std=c++23>") # why isn't this being set?!
if (pie_supported)
  set_property(TARGET ein_tests PROPERTY POSITION_INDEPENDENT_CODE TRUE)
  target_precompile_headers(ein_tests REUSE_FROM ein)
  set_target_properties(ein_tests PROPERTIES LINKER_LANGUAGE CXX)
endif()
target_link_libraries(ein_tests PRIVATE ein Catch2::Catch2WithMain)
lto(ein_tests)

include(Catch)
catch_discover_tests(ein_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
