/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

#include "bf16.hpp"

namespace std {
  template class numeric_limits<ein::bf16>;
}

/// \cond
#ifdef ENABLE_TESTS
#include <doctest.h>
#define CHECK_APPROX(val, expected, epsilon) CHECK(std::abs((val) - (expected)) < (epsilon))

using namespace ein;
using doctest::Approx;

TEST_SUITE("bf16") {

  TEST_CASE("bf16 default constructors and conversion") {
    CHECK(bf16(0.5f).content == Approx(0.5f));
    CHECK(0.25_bf16 .content == Approx(0.25f));
    CHECK(static_cast<float>(1.0_bf16) == Approx(1.0f));
  }

  TEST_CASE("bf16 comparison operators") {
    CHECK(0.5_bf16 == 0.5_bf16);
    CHECK(0.1_bf16 < 0.2_bf16);
    CHECK(0.3_bf16 > 0.2_bf16);
    CHECK(0.2_bf16 <= 0.2_bf16);
    CHECK(0.2_bf16 >= 0.2_bf16);

    // Negative checks for inequality
    CHECK_FALSE(0.5_bf16 < 0.5_bf16);
    CHECK_FALSE(0.3_bf16 < 0.2_bf16);
    CHECK_FALSE(0.1_bf16 > 0.2_bf16);
    CHECK_FALSE(0.3_bf16 <= 0.2_bf16);
    CHECK_FALSE(0.1_bf16 >= 0.2_bf16);
  }

  TEST_CASE("bf16 swap function") {
    bf16 x(1.0_bf16), y(2.0_bf16);
    swap(x, y);
    CHECK(x == 2.0_bf16);
    CHECK(y == 1.0_bf16);
  }

  TEST_CASE("bf16 bit operations") {
    bf16 val = bf16::from_bits(0x3F00);
    CHECK(val.to_bits() == 0x3F00);
    CHECK(bf16::from_bits(0x7F80) == std::numeric_limits<bf16>::infinity());
  }

  TEST_CASE("bf16 isnan function") {
    CHECK(std::isnan(bf16::from_bits(0x7FC0)));
    CHECK_FALSE(std::isnan(1.0_bf16));
  }

  TEST_CASE("bf16 numeric_limits") {
    using nl = std::numeric_limits<bf16>;

    CHECK(nl::is_specialized);
    CHECK(nl::is_signed);
    CHECK(nl::digits == 7);
    CHECK(nl::epsilon() == bf16::from_bits(0x3C00));
    CHECK(nl::round_error() == bf16::from_bits(0x3F00));
    CHECK(nl::min() == bf16::from_bits(0x007F));
    CHECK(nl::max() == bf16::from_bits(0x7F7F));
    CHECK(nl::lowest() == bf16::from_bits(0xFF7F));
    CHECK(nl::infinity() == bf16::from_bits(0x7F80));
    CHECK(std::isnan(nl::quiet_NaN()));
    CHECK(std::isnan(nl::signaling_NaN()));
    CHECK(nl::denorm_min() == bf16::from_bits(0));
  }

} // TEST_SUITE("bf16")

#endif
/// \endcond
