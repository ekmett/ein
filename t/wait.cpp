#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <atomic>
#include <thread>
#include <chrono>
#include "ein/wait.hpp" // Replace with the actual header file name

namespace ein {

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

} // namespace ein

TEST_CASE("wait_until with mock_waiter","[wait]") {
    using namespace ein;

    volatile int data = 0;
    auto predicate = [](volatile int* p) { return *p == 42; };

    std::thread updater([&data]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        data = 42;
    });

    wait_until<mock_waiter>(&data, predicate);

    REQUIRE(mock_waiter::monitor_called.load());
    REQUIRE(mock_waiter::mwait_called.load());
    REQUIRE(data == 42);

    updater.join();
}

TEST_CASE("wait_until with platform waiter","[wait]") {
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

    REQUIRE(data == 42);

    updater.join();
}
