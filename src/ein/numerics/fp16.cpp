
/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */


#include "fp16.hpp"

namespace std {
  template class numeric_limits<ein::fp16>;
}

/// \cond
#ifdef ENABLE_TESTS
#include <doctest.h>

static constexpr float epsilon = 1e-5;

#define CHECK_APPROX(val, expected) CHECK(std::abs((val) - (expected)) < (epsilon))

using namespace ein;
using doctest::Approx;

TEST_SUITE("fp16") {

  TEST_CASE("fp16 default constructors and conversion") {
    CHECK(fp16(0.5f).content == Approx(0.5f));
    CHECK(0.25_fp16 .content == Approx(0.25f));
    CHECK(static_cast<float>(1.0_fp16) == Approx(1.0f));
    CHECK(static_cast<_Float16>(0.75_fp16) == Approx(_Float16(0.75f)));
  }

  TEST_CASE("fp16 comparison operators") {
    CHECK(0.5_fp16 == 0.5_fp16);
    CHECK(0.1_fp16 < 0.2_fp16);
    CHECK(0.3_fp16 > 0.2_fp16);
    CHECK(0.2_fp16 <= 0.2_fp16);
    CHECK(0.2_fp16 >= 0.2_fp16);
  }

  TEST_CASE("fp16 swap function") {
    fp16 x(1.0_fp16), y(2.0_fp16);
    swap(x, y);
    CHECK(x == fp16(2.0_fp16));
    CHECK(y == fp16(1.0_fp16));
  }

  TEST_CASE("fp16 bit operations") {
    fp16 val = fp16::from_bits(0x3C00);
    CHECK(val.to_bits() == 0x3C00);
    CHECK(fp16::from_bits(0x7C00) == std::numeric_limits<fp16>::infinity());
  }

  TEST_CASE("fp16 isnan function") {
    CHECK(std::isnan(fp16::from_bits(0x7FFF)));
    CHECK_FALSE(std::isnan(fp16(1.0_fp16)));
  }

  TEST_CASE("fp16 numeric_limits") {
    using nl = std::numeric_limits<fp16>;

    CHECK(nl::is_specialized);
    CHECK(nl::is_signed);
    CHECK(nl::digits == 11);
    CHECK(nl::epsilon() == fp16::from_bits(0x1200));
    CHECK(nl::round_error() == fp16::from_bits(0x3C00));
    CHECK(nl::min() == fp16::from_bits(0x0200));
    CHECK(nl::max() == fp16::from_bits(0x7BFF));
    CHECK(nl::lowest() == fp16::from_bits(0xFBFF));
    CHECK(nl::infinity() == fp16::from_bits(0x7C00));
    CHECK(std::isnan(nl::quiet_NaN()));
    CHECK(std::isnan(nl::signaling_NaN()));
    CHECK(nl::denorm_min() == fp16::from_bits(0));
  }

} // TEST_SUITE("fp16")
#endif
/// \endcond
