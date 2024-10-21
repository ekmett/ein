module;

#include <sys/mman.h> // munmap
#include <limits>
#include <memory>
#include <string.h> // strdup
#include "attributes.hpp"

export module ein.memory;

namespace ein {

export template<typename T>
concept trivially_destructible
      = std::is_trivially_destructible_v<std::remove_extent_t<T>>;

export struct c_munmap {
  size_t size;
  void operator()(void * p) const noexcept {
    if (p != nullptr)
      munmap(p, size);
  }
};

export
using mmap_ptr = std::unique_ptr<void,c_munmap>;

export
EIN(nonnull(1))
mmap_ptr make_mmap_ptr(void * p, size_t size) {
  return mmap_ptr(p,c_munmap(size));
}

export
struct c_free {
  template <typename T>
  void operator()(T *p) const {
    std::free(const_cast<std::remove_const_t<T>*>(p));
  }
};

export
template<trivially_destructible T>
using unique_c_ptr = std::unique_ptr<T, c_free>;

static_assert(sizeof(char *)== sizeof(unique_c_ptr<char>),"");

export
using unique_str = unique_c_ptr<char const>;

export
EIN(nodiscard,inline,pure,nonnull(1),null_terminated_string_arg(1))
unique_str dup(const char * string) noexcept {
  return unique_str { strdup(string) };
}

} //namespace ein
