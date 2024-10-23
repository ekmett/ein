module;

#include <x86intrin.h>

export module ein.wait;

import ein.cpu;

namespace ein::wait {

/// some way to wait for a value to change
export template <typename T>
concept waiter = requires (void * p, uint32_t t) {
  bool(T::supported);
  T::monitor(p);
  T::mwait(t);
};

/// wait until a predicte no longer holds about a given memory location
/// may probe periodically, or after any write to the target cache line
/// timer is max mwait duration, it is not a timeout for now.
/// \post f(p)
export template <waiter W>
/// \flatten \cond
EIN(flatten) /// \endcond
void wait_until(auto * p, auto f, uint32_t timer = 0) {
  assume(W::supported);
  while (!f(p)) {
    W::monitor(p);
    if (!f(p))
      W::mwait(timer);
  }
}

export struct mwaitx {
  using timer_t = uint64_t;
  /// \inline \artificial \cond
  EIN(inline,artificial) /// \endcond
  static void monitor(void * p) noexcept { _mm_monitorx(p,0,0); }
  /// \inline \artificial \cond
  EIN(inline,artificial) /// \endcond
  static void mwait(uint32_t timer = 0) noexcept { _mm_mwaitx(0,0,timer); }
  static const bool supported;
};

export struct umwait {
  using timer_t = uint64_t;
  /// \inline \artificial \cond
  EIN(inline,artificial) /// \endcond
  static void monitor(void * p) noexcept { return _umonitor(p); }
  /// \inline \artificial \cond
  EIN(inline,artificial) /// \endcond
  static uint8_t mwait(uint32_t timer = 0) noexcept { return _umwait(1,timer); }
  static const bool supported;
};

export struct spin {
  using timer_t = uint64_t;
  /// \inline \artificial \cond
  EIN(inline,artificial) /// \endcond
  static void monitor(void *) noexcept {}
  /// \inline \artificial \cond
  EIN(inline,artificial) /// \endcond
  static void mwait(uint32_t = 0) noexcept { _mm_pause(); }
  static constexpr bool supported = true;
};

/// Usage: \code with_waiter([]<waiter w> { ...; wait_until<w>(p,f); ... }); \endcode
/// invokes \p k with a waiter as a template parameter.
export auto with_waiter(auto k) {
  if (mwaitx::supported) return k.template operator()<mwaitx>();
  else if (umwait::supported) return k.template operator()<umwait>();
  else k.template operator()<spin>();
}

} // ein::wait

namespace ein {
  using namespace ::ein::wait;
}
