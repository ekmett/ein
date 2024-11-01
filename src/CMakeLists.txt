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

# header jiujutsu
# ====================================================================

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/ein/config.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/ein/config.hpp"
)

file(GLOB_RECURSE SOURCE_HEADER_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

# list(APPEND SOURCE_HEADER_FILES
#   "${CMAKE_CURRENT_SOURCE_DIR}/ein/assert.hpp"
#   "${CMAKE_CURRENT_SOURCE_DIR}/ein/attributes.hpp"
#   "${CMAKE_CURRENT_SOURCE_DIR}/ein/concepts.hpp"
#   "${CMAKE_CURRENT_SOURCE_DIR}/ein/cpuid.hpp"
#   "${CMAKE_CURRENT_SOURCE_DIR}/ein/numerics.hpp"
#   "${CMAKE_CURRENT_SOURCE_DIR}/ein/numerics/fp16.hpp"
#   "${CMAKE_CURRENT_SOURCE_DIR}/ein/numerics/bf16.hpp"
#   "${CMAKE_CURRENT_SOURCE_DIR}/ein/memory.hpp"
#   "${CMAKE_CURRENT_SOURCE_DIR}/ein/profiling.hpp"
#   "${CMAKE_CURRENT_SOURCE_DIR}/ein/simd.hpp"
#   "${CMAKE_CURRENT_SOURCE_DIR}/ein/simd_data.hpp"
#   "${CMAKE_CURRENT_SOURCE_DIR}/ein/static_string.hpp"
#   "${CMAKE_CURRENT_SOURCE_DIR}/ein/std/forward_like.hpp"
#   "${CMAKE_CURRENT_SOURCE_DIR}/ein/types.hpp"
#   "${CMAKE_CURRENT_SOURCE_DIR}/ein/wait.hpp"
#)

# Replace SOURCE_DIR with BINARY_DIR for each header
set(OUTPUT_HEADER_FILES "")
foreach(header ${SOURCE_HEADER_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/include" replaced_header "${header}")
    list(APPEND OUTPUT_HEADER_FILES "${replaced_header}")
endforeach()

# Custom command to copy header files
#add_custom_command(
#  OUTPUT ${OUTPUT_HEADER_FILES}
#  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SOURCE_HEADER_FILES} "${CMAKE_CURRENT_BINARY_DIR}/h"
#  DEPENDS ${SOURCE_HEADER_FILES}
#)

foreach(header_file IN LISTS SOURCE_HEADER_FILES)
  file(RELATIVE_PATH relative_path "${CMAKE_CURRENT_SOURCE_DIR}" "${header_file}")
  set(target_path "${CMAKE_CURRENT_BINARY_DIR}/include/${relative_path}")
  get_filename_component(target_dir "${target_path}" DIRECTORY)
  file(MAKE_DIRECTORY "${target_dir}")
  add_custom_command(
    OUTPUT "${target_path}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${header_file}" "${target_path}"
    DEPENDS "${header_file}"
  )
  list(APPEND OUTPUT_HEADER_FILES "${target_path}")
endforeach()

list(APPEND OUTPUT_HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/include/ein/config.hpp)
add_custom_target(ein_headers ALL DEPENDS ${OUTPUT_HEADER_FILES})

add_library(ein STATIC
   ein/static_string.cpp
   ein/cpuid.cpp
   ein/wait.cpp
)

#  numerics.cpp
#  pretty.cpp


# set_target_properties(ein PROPERTIES CXX_SCAN_FOR_MODULES ON)
add_dependencies(ein ein_headers)

target_link_libraries(ein PUBLIC compiler_rt_builtins nlohmann_json::nlohmann_json spdlog::spdlog Threads::Threads)

target_include_directories(ein PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
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
