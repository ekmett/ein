# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

cmake_minimum_required(VERSION 3.30.5 FATAL_ERROR)
cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})


project(ein VERSION 0.0.1 LANGUAGES CXX)
set(PROJECT_BRIEF "一呼百應, \\\"one call, a hundred responses\\\"")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/cache")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
SET(SPDLOG_BUILD_PIC ON CACHE BOOL "Force PIC for SPDLOG")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_POSITION_INDEPENDENT_)
set(FETCHCONTENT_QUIET ON)  # Suppresses FetchContent output
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/etc/cmake")

include(CheckRestrict)
include(FetchContentCached)
include(CheckIPOSupported)
include(CheckPIESupported)

# pie
# ====================================================================

check_pie_supported(OUTPUT_VARIABLE pie_supported LANGUAGES CXX)
if (pie_supported)
  # message(STATUS "PIE is supported: ${pie_supported}")
else()
  # message(WARNING "PIE is not supported: ${pie_supported}")
endif()

# lto
# ====================================================================

check_ipo_supported(RESULT lto_supported OUTPUT lto_check_output)
if (lto_supported)
  set(ENV{LLVM_PARALLEL_LINK_JOBS} 32)
  set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(WARNING "IPO/LTO is not supported: ${lto_check_output}")
endif()

# tweak ccache settings
# ====================================================================

set(CMAKE_CXX_COMPILER_LAUNCHER "${CMAKE_CURRENT_SOURCE_DIR}/bin/ccache.sh")
set(ENV{CCACHE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/.ccache")

# options
# ====================================================================

option(ENABLE_TESTS "Enable tests" ON)
option(PRECOMPILE_HEADERS "Use precompiled headers" OFF)
option(BUILD_NATIVE "Build for current host only" ON)
option(USE_GOLD_LINKER "link with gold" ON)
option(ASAN "address sanitization" OFF)
option(TSAN "thread sanitization" OFF)
option(MSAN "memory sanitization" OFF)
option(UBSAN "undefined behavior sanitization" OFF)
option(AVX512 "Enable AVX512" ON)
SET(SPDLOG_BUILD_PIC ON CACHE BOOL "Force PIC for SPDLOG")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

set(CXX_Clang "$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>")
set(CXX_Gcc "$<COMPILE_LANG_AND_ID:CXX,GNU>")
set(CXX_GccOrClang "$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,GNU>")
set(Link_Clang "$<LINK_LANG_AND_ID:CXX,AppleClang,Clang>")
set(Link_Gcc "$<COMPILE_LANG_AND_ID:CXX,GNU>")
set(Link_GccOrClang "$<LINK_LANG_AND_ID:CXX,AppleClang,Clang,GNU>")

# add lots of debug info to Debug builds
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-O0 -g3 -ggdb3 -gcolumn-info -gembed-source")

include(CheckCXXCompilerFlag)

find_program(GOLD_LINKER ld.gold)
check_cxx_compiler_flag("-fuse-ld=gold" HAS_GOLD_FLAG)

set(AGGRESSIVE_LINKER_FLAGS
  "$<IF:$<AND:$<BOOL:${GOLD_LINKER}>,$<BOOL:${USE_GOLD_LINKER}>,$<BOOL:${HAS_GOLD_FLAG}>>,-fuse-ld=gold;-Wl\,--icf=all>;-Wl,--gc-sections;-Wl,--as-needed;-Wl,--relax;-Wl,--sort-common;-Wl,--hash-style=gnu;-Wl,-O2" # ;-Wl,--strip-debug
)

function(enable_aggressive_linking TARGET)
  target_link_options(${TARGET} PRIVATE ${AGGRESSIVE_LINKER_FLAGS})
endfunction()

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
      message(NOTICE "Disabling AVX512: ${FEATURE} not supported")
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
#macro(gc_sections)
#  foreach(name IN LISTS ARGN)
#    target_link_options(${name} PRIVATE $<${Link_GccOrClang}:-Wl,--gc-sections>)
#  endforeach()
#endmacro()

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

include(CheckRestrict)

# config.hpp
# ====================================================================

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/etc/config.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/ein/config.hpp"
)

# source
# ====================================================================

if(ENABLE_TESTS)
  include(CTest)
  enable_testing()
endif()


add_subdirectory(src)
add_subdirectory(doc)
