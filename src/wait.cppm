/// \file
/// \ingroup wait
/// \brief waiting strategies
/// \license
/// SPDX-FileType: Source
/// SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
/// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
/// \endlicense
module;

#include "prelude.hpp"
#include <x86intrin.h>

using namespace std;

/// \ingroup wait
export module ein.wait;

import ein.cpuid;

namespace ein {
/// \defgroup wait wait
/// \{

/// \brief some way to wait for a value to change
/// \tparam T type to check
export template <typename T>
concept waiter = requires (void * p, uint32_t t) {
  bool(T::supported);
  T::monitor(p);
  T::mwait(t);
};

/// \brief Wait until a predicate holds about a given memory location
/// \param p pointer to data. activity on its cache line will wake us up
/// \param f predicate to check about the pointer
///
/// \post f(p)
export template <waiter W> ein_flatten
void wait_until(auto * p, auto f) noexcept {
  assume(W::supported);
  while (!f(p)) {
    W::monitor(p);
    if (!f(p))
      W::mwait(0);
  }
}

/// \ref waiter using MONITORX/MWAITX for AMD
export struct mwaitx {
  using timer_t = uint64_t;
  ein_inline ein_artificial static void monitor(void * p) noexcept { _mm_monitorx(p,0,0); }
  ein_inline ein_artificial static void mwait(uint32_t timer = 0) noexcept { _mm_mwaitx(0,0,timer); }
  /// \hideinlinesource
  static const bool supported;
};

/// \hideinitializer \hideinlinesource
const bool mwaitx::supported = [] static noexcept {
  return (cpu_vendor == cpu_vendor::amd)
      && ((cpuid(0x80000001,0).ecx & (1 << 29)) != 0);
}();


/// \ref waiter using \UMONITOR/\UMWAIT for Intel
export struct umwait {
  using timer_t = uint64_t;
  ein_inline ein_artificial static void monitor(void * p) noexcept { return _umonitor(p); }
  ein_inline ein_artificial static uint8_t mwait(uint32_t timer = 0) noexcept { return _umwait(1,timer); }
  /// \hideinlinesource
  static const bool supported;
};

/// \hideinitializer \hideinlinesource
const bool umwait::supported = [] static noexcept {
  return (cpu_vendor == cpu_vendor::intel)
      && ((cpuid(0x7,0).ecx & (1 << 5)) != 0);
}();

/// spin \ref waiter using \PAUSE
export struct spin {
  using timer_t = uint64_t;
  ein_inline ein_artificial static void monitor(void *) noexcept {}
  ein_inline ein_artificial static void mwait(uint32_t = 0) noexcept { _mm_pause(); }
  /// \hideinlinesource
  inline static constinit bool supported = true;
};

/// \brief finds an appropriate waiter for the current CPU
/// \details Chooses between variations of user-mode \MONITOR / \MWAIT.
/// Usage:
/// \code{.cpp}
///   with_waiter([]<waiter w> noexcept { ...; wait_until<w>(p,f); ... });
/// \endcode
/// invokes \p k with a waiter as a template parameter.
export auto with_waiter(auto k) noexcept {
  if (mwaitx::supported) return k.template operator()<mwaitx>();
  else if (umwait::supported) return k.template operator()<umwait>();
  else k.template operator()<spin>();
}

/// \}
}
