# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

cmake_minimum_required(VERSION 3.30.5 FATAL_ERROR)
cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})

set(PROJECT_BRIEF "SIMD Tools")

# set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
# set(CMAKE_CXX_MODULE_STD TRUE)
set(CMAKE_EXPERIMENTAL_CXX_MODULES TRUE)

project(ein VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/lib/cache")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
SET(SPDLOG_BUILD_PIC ON CACHE BOOL "Force PIC for SPDLOG")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/etc/cmake")

include(CheckIPOSupported)
include(CheckRestrict)
#include(FetchContent)
include(FetchContentCached)


# ccache
# ====================================================================

set(FETCHCONTENT_QUIET ON)  # Suppresses FetchContent output

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  fetchcontentcached(
    NAME ccache
    URL https://github.com/ccache/ccache/releases/download/v4.10.2/ccache-4.10.2-linux-x86_64.tar.xz
    URL_HASH SHA256=80cab87bd510eca796467aee8e663c398239e0df1c4800a0b5dff11dca0b4f18
)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  fetchcontentcached(
    NAME ccache
    URL https://github.com/ccache/ccache/releases/download/v4.10.2/ccache-4.10.2-darwin.tar.gz
    URL_HASH SHA256=d90514fff15943a8607e84e3f42d45f823915a92f99984f3fc88202f6295d1e8
)
endif()

set(CMAKE_CXX_COMPILER_LAUNCHER "${CMAKE_SOURCE_DIR}/bin/ccache.sh")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(ENV{CCACHE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/.ccache")

# options
# ====================================================================

option(CODE_COVERAGE "Enable code coverage instrumentation" OFF)
option(ENABLE_TESTS "Enable tests" ON)
option(PRECOMPILE_HEADERS "Use precompiled headers" OFF)
option(BUILD_NATIVE "Build for current host only" ON)
option(ASAN "address sanitization" OFF)
option(TSAN "thread sanitization" OFF)
option(MSAN "memory sanitization" OFF)
option(UBSAN "undefined behavior sanitization" OFF)
option(AVX512 "Enable AVX512" ON)
SET(SPDLOG_BUILD_PIC ON CACHE BOOL "Force PIC for SPDLOG")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CXX_Clang "$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>")
set(CXX_Gcc "$<COMPILE_LANG_AND_ID:CXX,GNU>")
set(CXX_GccOrClang "$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,GNU>")
set(Link_Clang "$<LINK_LANG_AND_ID:CXX,AppleClang,Clang>")
set(Link_Gcc "$<COMPILE_LANG_AND_ID:CXX,GNU>")
set(Link_GccOrClang "$<LINK_LANG_AND_ID:CXX,AppleClang,Clang,GNU>")

# Define an option to specify the llvm-cov executable name
set(LLVM_COV_EXECUTABLE "llvm-cov-19 gcov" CACHE STRING "Specify the llvm-cov executable name")

# add lots of debug info to Debug builds
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-O0 -g3 -ggdb3 -gcolumn-info -gembed-source")

check_ipo_supported(RESULT lto_supported OUTPUT lto_check_output)
if (lto_supported)
  set(ENV{LLVM_PARALLEL_LINK_JOBS} 16)
else()
  message(WARNING "IPO/LTO is not supported: ${lto_check_output}")
endif()

# Use the restrict keyword check
message(STATUS "Detected restrict: ${RESTRICT_KEYWORD}")

if (BUILD_NATIVE)
  # Check for AVX512 support.
  execute_process(
    COMMAND lscpu
    COMMAND tr '[:space:]' '\\n'
    COMMAND sort
    COMMAND uniq
    COMMAND grep avx512
    OUTPUT_VARIABLE CPUINFO
  )
  # Normalize flag names to avoid AMD/Intel discrepancies.
  string(REPLACE "avx512_vbmi2" "avx512vbmi2" CPUINFO "${CPUINFO}")
  # Check for AVX512 features required by ein/CMakeLists.txt.
  set(REQUIRED_AVX512_FEATURES avx512f avx512dq avx512bw avx512vl avx512vbmi2)
  foreach(FEATURE ${REQUIRED_AVX512_FEATURES})
    if(NOT "${CPUINFO}" MATCHES "${FEATURE}")
      set(AVX512 OFF)
      message(WARNING "Disabling AVX512: ${FEATURE} not supported")
      break()
    endif()
  endforeach()
endif()

# sanitizers
# ====================================================================

if(ASAN)
  add_compile_options("$<${CXX_GccOrClang}:-fsanitize=address>")
  add_link_options("$<${Link_GccOrClang}:-fsanitize=address>")
endif(ASAN)

if(TSAN)
  add_compile_options("$<${CXX_GccOrClang}:-fsanitize=thread>")
  add_link_options("$<${Link_GccOrClang}:-fsanitize=thread>")
endif(TSAN)

if(MSAN)
  add_compile_options("$<${CXX_GccOrClang}:-fsanitize=memory>")
  add_link_options("$<${Link_GccOrClang}:-fsanitize=memory>")
endif(MSAN)

if(UBSAN)
  add_compile_options("$<${CXX_GccOrClang}:-fsanitize=undefined>")
  add_link_options("$<${Link_GccOrClang}:-fsanitize=undefined>")
endif(UBSAN)

# macros
# ====================================================================

macro(cover)
  foreach(name IN LISTS ARGN)
    target_compile_options(${name} PRIVATE
      $<$<AND:$<BOOL:${CODE_COVERAGE}>,$<CXX_COMPILER_ID:Clang>>:-fprofile-instr-generate;-fcoverage-mapping>
    )
  endforeach()
endmacro()


# exclude targets from the 'all' target, so they have to be explicitly called for
macro(exclude)
  foreach(name IN LISTS ARGN)
    set_target_properties(${name} PROPERTIES EXCLUDE_FROM_ALL TRUE)
  endforeach()
endmacro()

# use interprocedural optimization (-flto)
macro(lto)
  foreach(name IN LISTS ARGN)
    set_target_properties(${name} PROPERTIES INTERPROCEDURAL_OPTIMIZATION ${lto_supported})
    if (lto_supported)
      target_compile_options(${name} PRIVATE $<${CXX_Clang}:-flto=thin>)
      target_link_options(${name} PRIVATE $<${Link_Clang}:-flto=thin>)
    endif()
  endforeach()
endmacro()

# slight output size optimization
macro(gc_sections)
  foreach(name IN LISTS ARGN)
    target_link_options(${name} PRIVATE $<${Link_GccOrClang}:-Wl,--gc-sections>)
  endforeach()
endmacro()

# third-party
# ====================================================================

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
find_package(Doxygen)

find_library(COMPILER_RT_BUILTINS
  NAMES clang_rt.builtins-x86_64
        clang_rt.osx
  PATHS /usr/lib/llvm-19/lib/clang/19/lib/linux
        /usr/local/opt/llvm@19/lib/clang/19/lib/darwin
  NO_DEFAULT_PATH
)

if(NOT COMPILER_RT_BUILTINS)
  message(FATAL_ERROR "clang_rt library not found")
endif()

add_library(compiler_rt_builtins INTERFACE)
target_link_libraries(compiler_rt_builtins INTERFACE ${COMPILER_RT_BUILTINS})

fetchcontentcached(
  NAME spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.13.0
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)

# silence a third-party warning on clang++-18+
target_compile_options(spdlog PRIVATE
  $<$<AND:$<CXX_COMPILER_ID:Clang>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,18>>:-Wno-unused-but-set-variable>
)

# compile options
# ====================================================================

# Overwrite the options we set for sentencepiece with our own.  Rationales:
# - -Wno-deprecated-declarations because it triggers on std::get_temporary_buffer<size_t>
#   inside glibc's implementation of std::stable_sort, which we invoke from llama.hpp.
set_directory_properties(
  PROPERTIES COMPILE_OPTIONS
  "$<${CXX_GccOrClang}:-Wno-deprecated-declarations>")

# Better debugging information
# Valgrind only starts understanding DWARF 5 as of version 3.20, but we're
# on version 3.18
add_compile_options("$<${CXX_GccOrClang}:-gdwarf-4;-fno-omit-frame-pointer>")
add_link_options("$<${Link_GccOrClang}:-gdwarf-4;-fno-omit-frame-pointer>")

# To allow gc'ing unused sections of code.
# This will matter more as we move things out of headers
# and into proper .cpp files for compile speed.
add_compile_options("$<${CXX_GccOrClang}:-ffunction-sections;-fdata-sections>")

# useful warnings
add_compile_options(
  "$<${CXX_GccOrClang}:-Wall;-Wpedantic;-Wextra;-Wno-shadow>"
  "$<${CXX_Gcc}:-Wstrict-overflow>")

# experimental attributes
add_compile_options(
  "$<${CXX_GccOrClang}:-Wconsumed>"
  "$<${CXX_GccOrClang}:--start-no-unused-arguments;--rtlib=compiler-rt;--end-no-unused-arguments>")
  # "$<${CXX_GccOrClang}:-Wthread-safety>"
add_link_options("$<${Link_Clang}:--rtlib=compiler-rt>")

# tests
# ====================================================================

if(ENABLE_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(t)
endif(ENABLE_TESTS)

# header jiujutsu
# ====================================================================

include(CheckRestrict)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/h/config.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/h/config.hpp"
)

list(APPEND SOURCE_HEADER_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/h/assert.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/h/attributes.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/h/intrinsics.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/h/prelude.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/h/std.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/h/third-party.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/h/posix.hpp"
)

# Replace SOURCE_DIR with BINARY_DIR for each header
set(OUTPUT_HEADER_FILES "")
foreach(header ${SOURCE_HEADER_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" replaced_header "${header}")
    list(APPEND OUTPUT_HEADER_FILES "${replaced_header}")
endforeach()

# Custom command to copy header files
add_custom_command(
  OUTPUT ${OUTPUT_HEADER_FILES}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SOURCE_HEADER_FILES} "${CMAKE_CURRENT_BINARY_DIR}/h"
  DEPENDS ${SOURCE_HEADER_FILES}
)

list(APPEND OUTPUT_HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/h/config.hpp)
add_custom_target(ein_headers ALL DEPENDS ${OUTPUT_HEADER_FILES})

add_subdirectory(src)
add_subdirectory(doc)

# Code Coverage
# ====================================================================

# If CODE_COVERAGE is enabled, add a custom target to generate coverage reports
if(CODE_COVERAGE)
  message(STATUS "Code coverage instrumentation enabled")
  add_custom_target(coverage
    COMMAND llvm-profdata merge -sparse default.profraw -o coverage.profdata
    COMMAND ${LLVM_COV_EXECUTABLE} export --format=lcov --instr-profile=coverage.profdata $(find . -name "*.cpp.o" -o -name "*.cppm.o") > coverage.info
    COMMAND genhtml coverage.info --output-directory coverage-report
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating code coverage report"
  )
else()
  message(STATUS "Code coverage instrumentation disabled")
endif()
