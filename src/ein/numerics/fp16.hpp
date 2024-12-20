#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

#include <bit>
#include <compare>
#include <cstdint>
#include <limits>
#include "../attributes/common.hpp"

namespace ein {

struct fp16 {
  using underlying_type = _Float16;

  _Float16 content;

  ein_inline ein_artificial
  constexpr fp16() noexcept = default;

  ein_inline ein_artificial
  constexpr fp16(ein_noescape fp16 const &) noexcept = default;

  ein_inline ein_artificial
  constexpr fp16(ein_noescape fp16 &&) noexcept = default;

  ein_inline ein_artificial
  constexpr fp16(float content) noexcept : content(content) {}

  ein_inline ein_artificial
  constexpr fp16(_Float16 content) noexcept : content(content) {}

  ein_nodiscard ein_inline ein_artificial
  constexpr operator float (this fp16 self) noexcept { return self.content; }

  ein_nodiscard ein_inline ein_artificial
  constexpr operator _Float16 (this fp16 self) noexcept { return self.content; }

  ein_reinitializes ein_inline ein_artificial
  constexpr fp16 & operator = (ein_noescape fp16 const &) noexcept
  ein_lifetimebound = default;

  ein_reinitializes ein_inline ein_artificial
  constexpr fp16 & operator = (ein_noescape fp16 &&) noexcept
  ein_lifetimebound = default;

  ein_nodiscard ein_inline ein_artificial ein_const
  friend constexpr bool operator == (fp16 x, fp16 y) noexcept {
    return x.content == y.content;
  }

  ein_nodiscard ein_inline ein_artificial ein_const
  friend constexpr bool operator != (fp16 x, fp16 y) noexcept {
    return x.content != y.content;
  }

  ein_nodiscard ein_inline ein_artificial ein_const
  friend constexpr std::partial_ordering operator <=> (fp16 x, fp16 y) noexcept {
    return x.content <=> y.content;
  }

  ein_nodiscard ein_inline ein_artificial ein_const
  friend constexpr bool operator < (fp16 x, fp16 y) noexcept {
    return x.content < y.content;
  }

  ein_nodiscard ein_inline ein_artificial ein_const
  friend constexpr bool operator <= (fp16 x, fp16 y) noexcept {
    return x.content <= y.content;
  }
  ein_nodiscard ein_inline ein_artificial ein_const
  friend constexpr bool operator > (fp16 x, fp16 y) noexcept {
    return x.content > y.content;
  }

  ein_nodiscard ein_inline ein_artificial ein_const
  friend constexpr bool operator >= (fp16 x, fp16 y) noexcept {
    return x.content >= y.content;
  }

  ein_inline ein_artificial
  friend constexpr void swap(
    ein_noescape fp16 & x,
    ein_noescape fp16 & y
  ) noexcept {
    using std::swap;
    swap(x.content,y.content);
  }

  ein_inline ein_artificial ein_const
  static constexpr fp16 from_bits(uint16_t data) noexcept {
    return std::bit_cast<fp16>(data);
  }

  ein_nodiscard ein_inline ein_const
  constexpr uint16_t to_bits(this fp16 self) noexcept {
    return std::bit_cast<uint16_t>(self.content);
  }

};

ein_nodiscard ein_inline ein_artificial ein_const
constexpr fp16 operator""_fp16(long double v) noexcept {
  return fp16(static_cast<float>(v));
}

} // namespace ein

namespace std {
  ein_nodiscard ein_artificial ein_inline ein_const
  constexpr bool isnan(ein::fp16 x) noexcept {
    // Sign doesn't matter, frac not zero (infinity)
    return (x.to_bits() & 0x7FFF) > 0x7c00;
  }

  template <>
  class numeric_limits<ein::fp16> {
  public:
    static constexpr bool is_specialized = true;
    ein_inline ein_artificial ein_const
    static constexpr ein::fp16 min() noexcept {
      return ein::fp16::from_bits(0x0200);
    }
    ein_inline ein_artificial ein_const
    static constexpr ein::fp16 max() noexcept {
      return ein::fp16::from_bits(0x7BFF);
    }
    ein_inline ein_artificial ein_const
    static constexpr ein::fp16 lowest() noexcept {
      return ein::fp16::from_bits(0xFBFF);
    }
    static constexpr int digits = 11;
    static constexpr int digits10 = 3;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr int radix = 2;
    ein_inline ein_artificial ein_const
    static constexpr ein::fp16 epsilon() noexcept {
      return ein::fp16::from_bits(0x1200);
    }
    ein_inline ein_artificial ein_const
    static constexpr ein::fp16 round_error() noexcept {
      return ein::fp16::from_bits(0x3C00);
    }
    static constexpr int min_exponent = -13;
    static constexpr int min_exponent10 = -4;
    static constexpr int max_exponent = 16;
    static constexpr int max_exponent10 = 4;
    static constexpr bool has_infinity = true;
    static constexpr bool has_quiet_NaN = true;
    static constexpr bool has_signaling_NaN = true;
    static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;
    ein_inline ein_artificial ein_const
    static constexpr ein::fp16 infinity() noexcept {
      return ein::fp16::from_bits(0x7C00);
    }
    ein_inline ein_artificial ein_const
    static constexpr ein::fp16 quiet_NaN() noexcept {
      return ein::fp16::from_bits(0x7FFF);
    }
    ein_inline ein_artificial ein_const
    static constexpr ein::fp16 signaling_NaN() noexcept {
      return ein::fp16::from_bits(0x7DFF);
    }
    ein_inline ein_artificial ein_const
    static constexpr ein::fp16 denorm_min() noexcept {
      return ein::fp16::from_bits(0);
    }
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = false;
    static constexpr bool is_modulo = false;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_to_nearest;
  };

  extern template class numeric_limits<ein::fp16>;

} // namespace std

#if defined(EIN_TESTING) || defined(EIN_TESTING_FP16)
TEST_CASE("fp16","[fp16]") {
  using namespace ein;
  using Catch::Approx;

  SECTION("fp16 default constructors and conversion") {
    CHECK(fp16(0.5f).content == Approx(0.5f));
    CHECK(0.25_fp16 .content == Approx(0.25f));
    CHECK(static_cast<float>(1.0_fp16) == Approx(1.0f));
    CHECK(static_cast<_Float16>(0.75_fp16) == Approx(_Float16(0.75f)));
  }

  SECTION("fp16 comparison operators") {
    CHECK(0.5_fp16 == 0.5_fp16);
    CHECK(0.1_fp16 < 0.2_fp16);
    CHECK(0.3_fp16 > 0.2_fp16);
    CHECK(0.2_fp16 <= 0.2_fp16);
    CHECK(0.2_fp16 >= 0.2_fp16);
  }

  SECTION("fp16 swap function") {
    fp16 x(1.0_fp16), y(2.0_fp16);
    swap(x, y);
    CHECK(x == fp16(2.0_fp16));
    CHECK(y == fp16(1.0_fp16));
  }

  SECTION("fp16 bit operations") {
    fp16 val = fp16::from_bits(0x3C00);
    CHECK(val.to_bits() == 0x3C00);
    CHECK(fp16::from_bits(0x7C00) == std::numeric_limits<fp16>::infinity());
  }

  SECTION("fp16 isnan function") {
    CHECK(std::isnan(fp16::from_bits(0x7FFF)));
    CHECK_FALSE(std::isnan(fp16(1.0_fp16)));
  }

  SECTION("fp16 numeric_limits") {
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
}
#endif
