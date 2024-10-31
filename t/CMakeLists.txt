# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

set(CATCH_INSTALL_DOCS OFF CACHE BOOL "skip catch docs")

FetchContentCached(
  NAME Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.7.1 # or a later release
)
#FetchContent_MakeAvailable(Catch2)

# silence a third-party warning on clang++-18+
target_compile_options(Catch2 PRIVATE
  $<$<AND:$<CXX_COMPILER_ID:Clang>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,18>>:-Wno-unused-but-set-variable>
)

list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")

add_executable(t_fp16 t_fp16.cpp)
target_link_libraries(t_fp16 PRIVATE Catch2::Catch2WithMain ein)

add_executable(t_bf16 t_bf16.cpp)
target_link_libraries(t_bf16 PRIVATE Catch2::Catch2WithMain ein)

# Discover Catch2 tests
include(Catch)
catch_discover_tests(t_fp16)
catch_discover_tests(t_bf16)
