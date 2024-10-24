# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

add_library(ein STATIC)

set_target_properties(ein PROPERTIES CXX_SCAN_FOR_MODULES ON)

target_link_libraries(ein PUBLIC
  spdlog::spdlog
)

target_sources(ein PUBLIC FILE_SET CXX_MODULES FILES
  cpuid.cppm
  memory.cppm
  numerics.cppm
  simd.cppm
  simd_data.cppm
  std.cppm
  types.cppm
  wait.cppm
)

# find_package(Threads REQUIRED)
#  nlohmann_json::nlohmann_json
#  Threads::Threads

target_link_libraries(ein PUBLIC
  ${COMPILER_RT_BUILTINS}
)

target_include_directories(ein PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/h>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/h>
  $<INSTALL_INTERFACE:include/ein>)

install(TARGETS ein
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include/ein
  INCLUDES DESTINATION include/ein)

set_target_properties(ein PROPERTIES PUBLIC_HEADER "assert.hpp;attributes.hpp;config.hpp;intrinsics.hpp;prelude.hpp;std.hpp;third-party.hpp")

lto(ein)

if(PRECOMPILE_HEADERS)
  target_precompile_headers(ein PRIVATE
    ${CMAKE_SOURCE_DIR}/h/std.hpp
    ${CMAKE_SOURCE_DIR}/h/intrinsics.hpp
    ${CMAKE_SOURCE_DIR}/h/third-party.hpp
    ${CMAKE_BINARY_DIR}/h/config.hpp
    ${CMAKE_SOURCE_DIR}/h/attributes.hpp
    ${CMAKE_SOURCE_DIR}/h/assert.hpp
    ${CMAKE_SOURCE_DIR}/h/prelude.hpp
  )
  target_compile_definitions(ein PUBLIC EIN_PCH=1)
endif()

target_compile_definitions(ein PUBLIC EIN_RESTRICT=${RESTRICT_KEYWORD})

target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-std=c++23>") # why isn't this being set?!
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-msse4.2;-mavx;-mbmi2;-mfma;-mavx2;-ftree-vectorize;-funroll-loops>")
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-mavx512f;-mavx512dq;-mavx512bw;-mavx512vl;-mavx512vbmi2;-mavx512vp2intersect;-mavx512bf16>")
target_compile_options(ein PUBLIC "$<${CXX_GccOrClang}:-Wno-unqualified-std-cast-call>")
