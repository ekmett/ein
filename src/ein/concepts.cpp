/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

/// \cond
#ifdef ENABLE_TESTS
#include <doctest.h>
#include <string>
#include "concepts.hpp"
using doctest::Approx;

namespace {
// Sample functions using ein_ensures for test cases
static auto add_integers(std::integral auto x, std::integral auto y) {
  return ein_ensures(std::integral, x + y);
}

static auto add_floats(std::floating_point auto x, std::floating_point auto y) {
  return ein_ensures(std::floating_point, x + y);
}

template <typename T>
concept Stringifiable = requires(T a) {
  { std::to_string(a) } -> std::convertible_to<std::string>;
};

static auto stringifyable_addition(Stringifiable auto x, Stringifiable auto y) {
  return ein_ensures(Stringifiable, x + y);
}
}

TEST_SUITE("concepts") {

  TEST_CASE("Test integral addition with ein_ensures") {
    CHECK(add_integers(2, 3) == 5);
    CHECK(add_integers(-10, 4) == -6);
  }

  TEST_CASE("Test floating-point addition with ein_ensures") {
    CHECK(add_floats(1.5, 2.0) ==  Approx(3.5));
    CHECK(add_floats(-10.0, 4.5) == Approx(-5.5));

  }

  TEST_CASE("Test custom concept Stringifiable with ein_ensures") {
    CHECK(stringifyable_addition(10, 20) == 30);

  }

  TEST_CASE("Test ein_ensures with non-matching concept should compile-time fail") {
      // Intentionally left commented as examples of cases that should fail to compile
      // REQUIRE(add_integers(2.5, 3.5) == 6.0); // Should fail on non-integral inputs
      // REQUIRE(add_floats(1, 2) == 3);         // Should fail on non-floating point inputs
  }
} // TEST_SUITE("concepts")
#endif
/// \endcond
