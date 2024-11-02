#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

#include <type_traits>
#include <cstring> // strdup
#include <memory>
#include <sys/mman.h> // munmap
#include "attributes/common.hpp"
#include "attributes/strings.hpp"
#include "attributes/null_safety.hpp"

namespace ein {

/// \brief a type where the destructor does no work, and therefore it is safe to simply discard.
template<typename T>
concept trivially_destructible = std::is_trivially_destructible_v<std::remove_extent_t<T>>;

/// \brief std::unique_ptr Deleter for memory mapped data
struct c_munmap {
  size_t size;
  /// \hideinlinesource
  /// \pre \p p is a memory region mapped by `mmap` \p size bytes long
  /// \post \p p has been `munmap`ped
  ein_inline ein_artificial
  void operator()(void * p) const noexcept {
    if (p != nullptr)
      munmap(p, size);
  }
};

/// memory mapped data, managed by std::unique_ptr. calls munmap to free.
using mmap_ptr = std::unique_ptr<void,c_munmap>;

/// construct a mmap_ptr using a base pointer and its size for munmap
/// \pre \p p is non-null
/// \hideinlinesource
ein_inline ein_artificial ein_nonnull(1)
mmap_ptr make_mmap_ptr(void * p, size_t size) noexcept {
  return mmap_ptr(p,c_munmap(size));
}

/// \brief std::unique_ptr Deleter that calls free()
struct c_free {
  /// \hideinlinesource
  template <trivially_destructible T>
  ein_inline ein_artificial
  static void operator()(T * p) noexcept {
    std::free(const_cast<std::remove_const_t<T>*>(p));
  }
};

/// a unique_ptr managed c pointer, deleted by free()
template<trivially_destructible T>
using unique_c_ptr = std::unique_ptr<T, c_free>;

static_assert(sizeof(char *)== sizeof(unique_c_ptr<char>),"");

/// a c string, managed by unique_ptr
using unique_str = unique_c_ptr<char const>;

/// \brief duplicate a C string using `strdup` and manage it as a \ref unique_str
/// \pre \p string is non-null
/// \post result is a null-terminated c string to be cleaned up by free
/// \hideinlinesource
ein_nodiscard ein_inline ein_artificial ein_pure
ein_nonnull(1) ein_null_terminated_string_arg(1)
unique_str dup(
  ein_noescape char const * str
) noexcept {
  return unique_str { strdup(str) };
}

} // ein

#ifdef EIN_DOCTEST

TEST_SUITE("memory") {
  using namespace ein;

  TEST_CASE("unique_str manages strdup-allocated strings") {
    const char* original = "Hello, World!";
    unique_str managed_str = dup(original);

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

    mmap_ptr managed_mmap = make_mmap_ptr(ptr, size);
    CHECK(managed_mmap != nullptr);
  }
} // TEST_SUITE("memory")

#endif // EIN_DOCTEST
