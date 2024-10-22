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

/// \brief a type where the destructor does no work, and therefore it is safe to simply discard.
/// \ingroup memory_group
export template<typename T>
concept trivially_destructible = is_trivially_destructible_v<remove_extent_t<T>>;

/// \brief std::unique_ptr Deleter for memory mapped data
/// \ingroup memory_group
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
/// \ingroup memory_group
export
using mmap_ptr = unique_ptr<void,c_munmap>;

/// construct a mmap_ptr using a base pointer and its size for munmap
/// \ingroup memory_group
/// \hideinlinesource \inline \artificial
/// \pre \p p is non-null
export /** \cond */ EIN(artificial,nonnull(1),inline) /** \endcond */
mmap_ptr make_mmap_ptr(void * p, size_t size) noexcept {
  return mmap_ptr(p,c_munmap(size));
}

/// \ingroup memory_group
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
/// \ingroup memory_group
export template<trivially_destructible T>
using unique_c_ptr = unique_ptr<T, c_free>;

static_assert(sizeof(char *)== sizeof(unique_c_ptr<char>),"");

/// a c string, managed by unique_ptr
/// \ingroup memory_group
export using unique_str = unique_c_ptr<char const>;

/// duplicate a c string using strdup and manage it as a unique_str
/// \ingroup memory_group
/// \nodiscard \inline \artificial \pure
/// \pre \p string is non-null
/// \post result is a null-terminated c string to be cleaned up by free
export /** \cond */ EIN(nodiscard,inline,artificial,pure,nonnull(1),null_terminated_string_arg(1)) /** \endcond */
unique_str dup(EIN(noescape) char const * string) noexcept {
  return unique_str { strdup(string) };
}

// Intel
// -----

/// \brief User-land monitor for changes at the cache line containing the target address. Intel version
/// \pre cpu::vendor == cpu::vendor::intel
/// \ingroup memory_group
/// \hideinlinesource
/// \showrefs
/** \cond */ EIN(inline,artificial) /** \endcond */
void monitor(void const * addr) noexcept {
  // MONITOR: rAX = addr, ECX = extensions, EDX = hints
  __asm__ __volatile__ (
    "monitor" // 0f 01 fa
    : // No output
    : "rax"(addr), "rcx"(0), "rdx"(0) // Input: addr, ECX = 0, EDX = 0
    : "memory" // Clobbered registers (memory is implied by monitor)
  );
}

/// \brief user-land mwait. intel version
/// while it isn't _documented_ to support the timer options from AMD on newer cpus it does.
/// \pre cpu::vendor == cpu::vendor::intel
/// \ingroup memory_group
/// \hideinlinesource
/// \showrefs
/** \cond */ EIN(inline,artificial) /** \endcond */
void mwait(uint32_t timer = 0) noexcept {
  __asm__ volatile("mwait" : : "a"(0), "c"(0), "d"(timer));
}

// AMD
// ---

/// \brief user-land monitor for changes at the cache line containing the target address. AMD version
/// \ingroup memory_group
/// \pre cpu::vendor == cpu::vendor::amd
/// \hideinlinesource
/// \showrefs
/** \cond */ EIN(inline,artificial) /** \endcond */
void monitorx(void const * addr) noexcept {
  // MONITORX: rAX = addr, ECX = extensions, EDX = hints
  __asm__ __volatile__ (
    "monitorx" // 0f 01 fa
    : // No output
    : "rax"(addr), "rcx"(0), "rdx"(0) // Input: addr, ECX = 0, EDX = 0
    : "memory" // Clobbered registers (memory is implied by monitorx)
  );
}

/// \brief user-land mwait. AMD version
/// \ingroup memory_group
/// \pre cpu::vendor == cpu::vendor::amd
/// \hideinlinesource
/// \showrefs
/** \cond */ EIN(inline,artificial) /** \endcond */
void mwaitx(uint32_t timer = 0) noexcept {
  __asm__ volatile("mwaitx" : : "a"(0), "c"(0), "d"(timer));
}

/// \brief spin using the flavor of MONITOR/MWAIT that matches your CPU until the target passes a user
/// supplied test \p f. Waiting happens \p timer cycles at a time.
///
/// \details
/// Waits on the entire cache line containing memory.
/// Automatically selects between Intel \ref monitor / \ref mwait and AMD \ref monitorx / \ref mwaitx.
///
/// By using MONITOR/MWAIT under the hood, this can achieve ~100ns wakeup times, while still allowing
/// the paired hyperthread to proceed with tasks unburdened by a heavy spin-wait.
///
/// \post \p f(\p m)
/// \ingroup memory_group
/// \hideinlinesource
export
template <typename T, typename F>
/** \cond */ EIN(flatten) /** \endcond */
void wait_until(T const & m, F f, uint32_t timer = 0) noexcept
requires requires(F f, T * p) { { f(p) } -> convertible_to<bool>; } {
  if (cpu::has_mwaitx) // amd
    while (!f(m)) {
      monitorx(&m);
      if (!f(m))
        mwaitx(&m,timer);
    }
  else {
    ein_assert(cpu::has_mwait); // intel
    while (!f(m)) {
      monitor(&m);
      if (!f(m))
        mwait(&m,timer);
    }
  }
}

} // ein::memory

export namespace ein {
  using namespace ::ein::memory;
} // ein
