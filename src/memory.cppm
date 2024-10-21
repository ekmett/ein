module;

/// \file
/// \brief memory primitives
/// \author Edward Kmett

#include <sys/mman.h> // munmap
#include <string.h> // strdup

using namespace std;

/// \defgroup memory_group memory
/// \brief memory primitives

export module ein.memory;

import ein.cpu;

namespace ein::memory {

/// \ingroup memory_group
export template<typename T>
concept trivially_destructible
  = is_trivially_destructible_v<remove_extent_t<T>>;

export struct c_munmap {
  size_t size;
  void operator()(void * p) const noexcept {
    if (p != nullptr)
      munmap(p, size);
  }
};

/// \ingroup memory_group
export
using mmap_ptr = unique_ptr<void,c_munmap>;

/// \ingroup memory_group
export
EIN(nonnull(1))
mmap_ptr make_mmap_ptr(void * p, size_t size) noexcept {
  return mmap_ptr(p,c_munmap(size));
}

export struct c_free {
  template <typename T>
  static void operator()(T * p) noexcept {
    free(const_cast<remove_const_t<T>*>(p));
  }
};

/// \ingroup memory_group
export template<trivially_destructible T>
using unique_c_ptr = unique_ptr<T, c_free>;

static_assert(sizeof(char *)== sizeof(unique_c_ptr<char>),"");

/// \ingroup memory_group
export using unique_str = unique_c_ptr<char const>;

/// \ingroup memory_group
export EIN(nodiscard,inline,pure,nonnull(1),null_terminated_string_arg(1))
unique_str dup(char const * string) noexcept {
  return unique_str { strdup(string) };
}

// Intel
// -----

/// \internal
EIN(export,inline)
void monitor(void const * addr) noexcept {
  // MONITOR: rAX = addr, ECX = extensions, EDX = hints
  __asm__ __volatile__ (
    "monitor" // 0f 01 fa
    : // No output
    : "rax"(addr), "rcx"(0), "rdx"(0) // Input: addr, ECX = 0, EDX = 0
    : "memory" // Clobbered registers (memory is implied by monitor)
  );
}

/// \internal
/// intel provides mwait. while it isn't _documented_ to support the timer options from AMD
/// on newer cpus it does.
EIN(export,inline)
void mwait(uint32_t timer = 0) noexcept {
  __asm__ volatile("mwait" : : "a"(0), "c"(0), "d"(timer));
}

// AMD
// ---

// \internal
EIN(export,inline)
void monitorx(void const * addr) noexcept {
  // MONITORX: rAX = addr, ECX = extensions, EDX = hints
  __asm__ __volatile__ (
    "monitorx" // 0f 01 fa
    : // No output
    : "rax"(addr), "rcx"(0), "rdx"(0) // Input: addr, ECX = 0, EDX = 0
    : "memory" // Clobbered registers (memory is implied by monitorx)
  );
}

// AMD provides mwaitx for userland mwait instead of mwait.
EIN(export,inline)
void mwaitx(uint32_t timer = 0) noexcept {
  __asm__ volatile("mwaitx" : : "a"(0), "c"(0), "d"(timer));
}

// spin using mwait/mwaitx watching memory until the target passes a test, waits timer-many cycles at a time.
// note: waits on the cache line containing memory.
void wait_until(auto const & m, auto p, uint32_t timer = 0) noexcept
requires requires(decltype(p) p, decltype(*m) m) { { t(m) } -> convertible_to<bool>; } {
  if (cpu::has_mwaitx) // amd
    while (!p(m)) {
      monitorx(&m);
      if (!p(m))
        mwaitx(&m,timer);
    }
  else {
    ein_assert(cpu::has_mwait); // intel
    while (!p(m)) {
      monitor(&m);
      if (!p(m))
        mwait(&m,timer);
    }
  }
}

} // ein::memory

export namespace ein {
  using namespace ::ein::memory;
} // ein
