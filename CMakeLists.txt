cmake_minimum_required(VERSION 3.30.5 FATAL_ERROR)
cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})

#set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
set(CMAKE_EXPERIMENTAL_CXX_MODULES TRUE)
# set(CMAKE_CXX_MODULE_STD TRUE)

project(ein VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
SET(SPDLOG_BUILD_PIC ON CACHE BOOL "Force PIC for SPDLOG")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Specify the required Doxygen version for cmake module support
set(REQUIRED_DOXYGEN_VERSION "1.12.0")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CheckIPOSupported)
include(FetchContent)
# include(ExternalProject)

# options
# =======

option(ENABLE_TESTS "Enable tests" ON)
option(PRECOMPILE_HEADERS "Use precompiled headers" ON)
option(BUILD_NATIVE "Build for current host only" ON)
option(ASAN "address sanitization" OFF)
option(TSAN "thread sanitization" OFF)
option(MSAN "memory sanitization" OFF)
option(UBSAN "undefined behavior sanitization" OFF)
option(AVX512 "Enable AVX512" ON)
SET(SPDLOG_BUILD_PIC ON CACHE BOOL "Force PIC for SPDLOG")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# place all executables in the top level binary directory so we can find them
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CXX_Clang "$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>")
set(CXX_Gcc "$<COMPILE_LANG_AND_ID:CXX,GNU>")
set(CXX_GccOrClang "$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,GNU>")
set(Link_Clang "$<LINK_LANG_AND_ID:CXX,AppleClang,Clang>")
set(Link_Gcc "$<COMPILE_LANG_AND_ID:CXX,GNU>")
set(Link_GccOrClang "$<LINK_LANG_AND_ID:CXX,AppleClang,Clang,GNU>")

# add lots of debug info to Debug builds
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-O0 -g3 -ggdb3 -gcolumn-info -gembed-source")

check_ipo_supported(RESULT lto_supported OUTPUT lto_check_output)
if (lto_supported)
  set(ENV{LLVM_PARALLEL_LINK_JOBS} 16)
else()
  message(WARNING "IPO/LTO is not supported: ${lto_check_output}")
endif()

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
# ==========

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
# ======

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
# ===========

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
find_package(Doxygen)

find_library(COMPILER_RT_BUILTINS
  NAMES clang_rt.builtins-x86_64
  PATHS /usr/lib/llvm-19/lib/clang/19/lib/linux
  NO_DEFAULT_PATH
)

if(NOT COMPILER_RT_BUILTINS)
  message(FATAL_ERROR "clang_rt.builtins-x86_64 library not found")
endif()

# spdlog is being used as our standard logging framework
fetchcontent_declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.13.0
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)
fetchcontent_makeavailable(spdlog)
# silence a third-party warning on clang++-18+
target_compile_options(spdlog PRIVATE
  $<$<AND:$<CXX_COMPILER_ID:Clang>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,18>>:-Wno-unused-but-set-variable>
)


# nlohmann's json library provides a solid c++ API for working with json
#fetchcontent_declare(
#  json
#  GIT_REPOSITORY https://github.com/nlohmann/json.git
#  GIT_TAG        v3.11.3
#  GIT_SHALLOW TRUE
#  GIT_PROGRESS TRUE
#)
#fetchcontent_makeavailable(json)

# lightweight C++ option parser library, supporting the standard GNU style syntax for options
#fetchcontent_declare(
#  cxxopts
#  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
#  GIT_TAG        v3.2.1
#  GIT_SHALLOW TRUE
#  GIT_PROGRESS TRUE
#)
#fetchcontent_makeavailable(cxxopts)

# compile options
# ===============

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
# =====

if(ENABLE_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(t)
endif(ENABLE_TESTS)

add_subdirectory(src)

# docs
# ====

find_package(Doxygen)
# Check Doxygen version

function(check_doxygen_version)
  if(DOXYGEN_FOUND)
    execute_process(
      COMMAND "${DOXYGEN_EXECUTABLE}" --version
      OUTPUT_VARIABLE DOXYGEN_VERSION_OUTPUT
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(DOXYGEN_VERSION_OUTPUT VERSION_LESS "${REQUIRED_DOXYGEN_VERSION}")
      set(DOXYGEN_FOUND FALSE PARENT_SCOPE)
    else()
      set(DOXYGEN_FOUND TRUE PARENT_SCOPE)
      set(DOXYGEN_VERSION "${DOXYGEN_VERSION_OUTPUT}" PARENT_SCOPE)
    endif()
  else()
    set(DOXYGEN_FOUND FALSE PARENT_SCOPE)
  endif()
endfunction()

# Check if the required Doxygen version is available
check_doxygen_version()

# Fetch Doxygen using FetchContent

if(DOXYGEN_FOUND)
  message(STATUS "Found Doxygen ${DOXYGEN_VERSION}: ${DOXYGEN_EXECUTABLE}")

  # Configure Doxyfile
  set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

  # Add a custom target for generating documentation
  add_custom_target(
    docs
    ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen ${DOXYGEN_EXECUTABLE}"
    VERBATIM
  )

  # Make sure the documentation target depends on DoxygenProject if we built it
  if(TARGET DoxygenProject)
    add_dependencies(docs DoxygenProject)
  endif()
endif()
