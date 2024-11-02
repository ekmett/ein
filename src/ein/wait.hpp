#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \brief waiting strategies

      \ingroup wait */

#include <cstdint>
#include <x86intrin.h>
#include "attributes.hpp"
#include "cpuid.hpp"

namespace ein {
/// \defgroup wait Waiting
/// \{

/// \brief some way to wait for a value to change
/// \tparam T type to check
template <typename T>
concept waiter = requires (volatile void * p, uint32_t t) {
  bool(T::supported);
  T::monitor(p);
  T::mwait(t);
};

/// \brief Wait until a predicate holds about a given memory location
/// \param p pointer to data. activity on its cache line will wake us up
/// \param f predicate to check about the pointer
///
/// \post f(p)
template <waiter W> ein_flatten
void wait_until(volatile auto * p, auto f) noexcept ein_blocking {
  [[assume(W::supported)]];
  while (!f(p)) {
    W::monitor(p);
    if (!f(p))
      W::mwait(0);
  }
}

/// \ref waiter using MONITORX/MWAITX for AMD
struct mwaitx {
  using timer_t = uint64_t;
  ein_inline ein_artificial static void monitor(volatile void * p) noexcept { _mm_monitorx(const_cast<void*>(p),0,0); }
  ein_inline ein_artificial static void mwait(uint32_t timer = 0) noexcept ein_blocking { _mm_mwaitx(0,0,timer); }
  /// \hideinitializer \hideinlinesource
  static const bool supported;
};


/// \ref waiter using \UMONITOR/\UMWAIT for Intel
struct umwait {
  using timer_t = uint64_t;
  ein_inline ein_artificial static void monitor(volatile void * p) noexcept { return _umonitor(const_cast<void*>(p)); }
  ein_inline ein_artificial static uint8_t mwait(uint32_t timer = 0) noexcept ein_blocking { return _umwait(1,timer); }
  /// \hideinlinesource
  static const bool supported;
};

/// spin \ref waiter using \PAUSE
struct spin {
  using timer_t = uint64_t;
  ein_inline ein_artificial static void monitor(volatile void *) noexcept {}
  ein_inline ein_artificial static void mwait(uint32_t = 0) noexcept ein_blocking { _mm_pause(); }
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
auto with_waiter(auto k) noexcept {
  if (mwaitx::supported) return k.template operator()<mwaitx>();
  else if (umwait::supported) return k.template operator()<umwait>();
  else k.template operator()<spin>();
}

/// \}
}

#ifdef EIN_DOCTEST
#include <atomic>
#include <thread>
#include <chrono>

TEST_SUITE("wait") {
  // Mock waiter that simulates waiting behavior
  struct mock_waiter {
    static inline bool supported = true;
    static inline std::atomic<bool> monitor_called = false;
    static inline std::atomic<bool> mwait_called = false;

    static void monitor(volatile void*) noexcept {
      monitor_called.store(true);
    }

    static void mwait(uint32_t timer = 0) noexcept {
      mwait_called.store(true);
      std::this_thread::sleep_for(std::chrono::milliseconds(timer));
    }
  };

  TEST_CASE("wait_until with mock_waiter") {
    using namespace ein;

    volatile int data = 0;
    auto predicate = [](volatile int* p) { return *p == 42; };

    std::thread updater([&data]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      data = 42;
    });

    wait_until<mock_waiter>(&data, predicate);

    CHECK(mock_waiter::monitor_called.load());
    CHECK(mock_waiter::mwait_called.load());
    CHECK(data == 42);

    updater.join();
  }

  TEST_CASE("wait_until with platform waiter") {
    using namespace ein;

    volatile int data = 0;
    auto predicate = [](volatile int* p) { return *p == 42; };

    std::thread updater([&data]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      data = 42;
    });

    with_waiter([&data,predicate] <waiter w> {
      wait_until<w>(&data, predicate);
    });

    CHECK(data == 42);

    updater.join();
  }
} // TEST_SUITE("wait")
#endif // EIN_DOCTEST
