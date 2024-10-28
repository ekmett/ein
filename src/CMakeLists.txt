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


add_library(ein STATIC
  numerics.cpp
)

set_target_properties(ein PROPERTIES CXX_SCAN_FOR_MODULES ON)
add_dependencies(ein ein_headers)

target_link_libraries(ein PUBLIC compiler_rt_builtins nlohmann_json::nlohmann_json spdlog::spdlog Threads::Threads)

target_sources(ein PUBLIC FILE_SET CXX_MODULES FILES
  cpuid.cppm
  memory.cppm
  numerics.cppm
 # fp16.cppm
  profiling.cppm
  simd.cppm
  simd_data.cppm
  static_string.cppm
  std.cppm
  types.cppm
  wait.cppm
)

target_include_directories(ein PUBLIC
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/h>
  $<INSTALL_INTERFACE:include/ein>)

install(TARGETS ein
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include/ein
  INCLUDES DESTINATION include/ein)

set_target_properties(ein PROPERTIES PUBLIC_HEADER "${OUTPUT_HEADER_FILES}")

lto(ein)

# PRECOMPILE_HEADERS (EIN_PCH=1) is the fastest
# accurate headers is a near second
# PRE
# EIN_PUBLIC=1 is far slower


if(PRECOMPILE_HEADERS)
  target_precompile_headers(ein PRIVATE ${OUTPUT_HEADER_FILES})
  target_compile_definitions(ein PUBLIC EIN_PCH=1)
else()
  target_compile_definitions(ein PUBLIC EIN_PRELUDE=1)
endif()

target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-std=c++23>") # why isn't this being set?!
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-msse4.2;-mavx;-mbmi2;-mfma;-mavx2;-ftree-vectorize;-funroll-loops>")
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-mavx512f;-mavx512dq;-mavx512bw;-mavx512vl;-mavx512vbmi2;-mavx512vp2intersect;-mavx512bf16>")
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-Wno-unqualified-std-cast-call>")
