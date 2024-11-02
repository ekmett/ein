/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

#include <doctest.h>
#include <atomic>
#include <thread>
#include <chrono>
#include "wait.hpp"

// Mock waiter that simulates waiting behavior
namespace {
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
}

TEST_SUITE("wait") {

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
