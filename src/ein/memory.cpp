
/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

/// \cond
#ifdef ENABLE_TESTS
#include "memory.hpp"
#include <doctest.h>


TEST_SUITE("memory") {
  TEST_CASE("unique_str manages strdup-allocated strings") {
    const char* original = "Hello, World!";
    ein::unique_str managed_str = ein::dup(original);

    CHECK(managed_str != nullptr);
    CHECK(std::strcmp(managed_str.get(), original) == 0);
  }

  TEST_CASE("unique_c_ptr manages malloc-allocated memory") {
    constexpr size_t size = 1024;
    ein::unique_c_ptr<char> managed_ptr(static_cast<char*>(std::malloc(size)));

    CHECK(managed_ptr != nullptr);
    std::memset(managed_ptr.get(), 0, size);
  }

  TEST_CASE("mmap_ptr manages mmap-allocated memory") {
    constexpr size_t size = 4096;
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    CHECK(ptr != MAP_FAILED);

    ein::mmap_ptr managed_mmap = ein::make_mmap_ptr(ptr, size);
    CHECK(managed_mmap != nullptr);
  }
} // TEST_SUITE("memory")
#endif
/// \endcond
