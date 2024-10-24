module;

/// \file
/// \ingroup memory
/// \license
/// SPDX-FileType: Source
/// SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
/// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
/// \endlicense

#include "prelude.hpp"
#include <sys/mman.h> // munmap
#include <cstring> // strdup

using namespace std;

export module ein;

namespace ein {
/// \defgroup memory memory
/// \{

/// \brief a type where the destructor does no work, and therefore it is safe to simply discard.
export template<typename T>
concept trivially_destructible = is_trivially_destructible_v<remove_extent_t<T>>;

/// \brief std::unique_ptr Deleter for memory mapped data
export struct c_munmap {
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
export using mmap_ptr = unique_ptr<void,c_munmap>;

/// construct a mmap_ptr using a base pointer and its size for munmap
/// \pre \p p is non-null
/// \hideinlinesource
export ein_inline ein_artificial ein_nonnull(1)
mmap_ptr make_mmap_ptr(void * p, size_t size) noexcept {
  return mmap_ptr(p,c_munmap(size));
}

/// \brief std::unique_ptr Deleter that calls free()
export struct c_free {
  /// \hideinlinesource
  template <trivially_destructible T>
  ein_inline ein_artificial
  static void operator()(T * p) noexcept {
    free(const_cast<remove_const_t<T>*>(p));
  }
};

/// a unique_ptr managed c pointer, deleted by free()
export template<trivially_destructible T>
using unique_c_ptr = unique_ptr<T, c_free>;

static_assert(sizeof(char *)== sizeof(unique_c_ptr<char>),"");

/// a c string, managed by unique_ptr
export using unique_str = unique_c_ptr<char const>;

/// duplicate a C string using `strdup` and manage it as a \ref unique_str
/// \pre \p string is non-null
/// \post result is a null-terminated c string to be cleaned up by free
/// \hideinlinesource
export ein_nodiscard ein_inline ein_artificial ein_pure
ein_nonnull(1) ein_null_terminated_string_arg(1)
unique_str dup(
  ein_noescape char const * string
) noexcept {
  return unique_str { strdup(string) };
}

/// \}

} // ein
