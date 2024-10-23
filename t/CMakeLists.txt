# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

include(FetchContent)

set(CATCH_INSTALL_DOCS OFF CACHE BOOL "skip catch docs")

if(NOT EXISTS "${FETCHCONTENT_BASE_DIR}/catch2-src")
  fetchcontent_declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1 # or a later release
  )
  fetchcontent_makeavailable(Catch2)
  # silence a third-party warning on clang++-18+
  target_compile_options(Catch2 PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,18>>:-Wno-unused-but-set-variable>
  )
else()
    set(catch2_SOURCE_DIR "${FETCHCONTENT_BASE_DIR}/catch2-src")
endif()

exclude(Catch2::Catch2 Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)
include(CatchShardTests)

# install(TARGETS t_simd RUNTIME DESTINATION bin)
