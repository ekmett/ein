#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <concepts>
#include <string>
#include "ein/concepts.hpp"

// Sample functions using ein_ensures for test cases
auto add_integers(std::integral auto x, std::integral auto y) {
    return ein_ensures(std::integral, x + y);
}

auto add_floats(std::floating_point auto x, std::floating_point auto y) {
    return ein_ensures(std::floating_point, x + y);
}

template <typename T>
concept Stringifiable = requires(T a) {
    { std::to_string(a) } -> std::convertible_to<std::string>;
};

auto stringifyable_addition(Stringifiable auto x, Stringifiable auto y) {
    return ein_ensures(Stringifiable, x + y);
}

// Test cases
TEST_CASE("Test integral addition with ein_ensures", "[ein_ensures]") {
    REQUIRE(add_integers(2, 3) == 5);
    REQUIRE(add_integers(-10, 4) == -6);

    // Check that ein_ensures fails to compile if given non-integrals
    // Uncommenting below line should trigger a static assertion failure
    // REQUIRE(add_integers(1.5, 2.0) == 3.5);
}

TEST_CASE("Test floating-point addition with ein_ensures", "[ein_ensures]") {
    REQUIRE(add_floats(1.5, 2.0) == Catch::Approx(3.5));
    REQUIRE(add_floats(-10.0, 4.5) == Catch::Approx(-5.5));

    // Uncommenting below line should trigger a static assertion failure
    // REQUIRE(add_floats(1, 2) == 3);
}

TEST_CASE("Test custom concept Stringifiable with ein_ensures", "[ein_ensures]") {
    REQUIRE(stringifyable_addition(10, 20) == 30);

    // Uncommenting below line should trigger a static assertion failure
    // REQUIRE(stringifyable_addition(1.5, 2.0) == 3.5);
}

TEST_CASE("Test ein_ensures with non-matching concept should compile-time fail", "[ein_ensures]") {
    // Intentionally left commented as examples of cases that should fail to compile
    // REQUIRE(add_integers(2.5, 3.5) == 6.0); // Should fail on non-integral inputs
    // REQUIRE(add_floats(1, 2) == 3);         // Should fail on non-floating point inputs
}

