/// \file
/// \ingroup memory
/// \license
/// SPDX-FileType: Source
/// SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
/// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
/// \endlicense

module;

#include <sys/mman.h> // munmap
#include <string.h> // strdup

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
  /// \hideinlinesource \inline \artificial
  /// \pre \p p is a memory region mapped by `mmap` \p size bytes long
  /// \post \p p has been `munmap`ped
  /** \cond */ EIN(inline,artificial) /** \endcond */
  void operator()(void * p) const noexcept {
    if (p != nullptr)
      munmap(p, size);
  }
};

/// memory mapped data, managed by std::unique_ptr. calls munmap to free.
export
using mmap_ptr = unique_ptr<void,c_munmap>;

/// construct a mmap_ptr using a base pointer and its size for munmap
/// \hideinlinesource \inline \artificial
/// \pre \p p is non-null
export /** \cond */ EIN(artificial,nonnull(1),inline) /** \endcond */
mmap_ptr make_mmap_ptr(void * p, size_t size) noexcept {
  return mmap_ptr(p,c_munmap(size));
}

/// \brief std::unique_ptr Deleter that calls free()
export struct c_free {
  /// \hideinlinesource \inline \artificial
  template <trivially_destructible T>
  /** \cond */ EIN(inline,artificial) /** \endcond */
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

/// duplicate a c string using strdup and manage it as a unique_str
/// \nodiscard \inline \artificial \pure
/// \pre \p string is non-null
/// \post result is a null-terminated c string to be cleaned up by free
export /** \cond */ EIN(nodiscard,inline,artificial,pure,nonnull(1),null_terminated_string_arg(1)) /** \endcond */
unique_str dup(EIN(noescape) char const * string) noexcept {
  return unique_str { strdup(string) };
}

/// \}
} // ein
