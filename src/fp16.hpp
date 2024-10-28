/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

module;

#if EIN_PRELUDE
#include "prelude.hpp"
#elifndef EIN_PCH
#include <bit>
#include <compare>
#include <cstdint>
#include <limits>
#include "attributes.hpp"
#endif

using namespace std;

export module ein.fp16;

namespace ein {

export struct fp16 {
  _Float16 content;

  ein_inline ein_artificial
  constexpr fp16() noexcept = default;

  ein_inline ein_artificial
  constexpr fp16(float content) noexcept : content(content) {}

  ein_nodiscard ein_inline ein_artificial
  constexpr operator float () const noexcept { return float(content); }

  ein_nodiscard ein_inline ein_artificial ein_const
  friend constexpr bool operator == (fp16 x, fp16 y) noexcept {
    return x.content == y.content;
  }

  ein_nodiscard ein_inline ein_artificialein_const
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
  friend constexpr void swap(fp16 & x, fp16 & y) noexcept {
    using std::swap;
    swap(x.content,y.content);
  }

  ein_inline ein_artificial ein_const
  static constexpr fp16 from_bits(uint16_t data) noexcept {
    return std::bit_cast<fp16>(data);
  }

  ein_nodiscard ein_inline ein_const
  constexpr uint16_t to_bits() const noexcept {
    return std::bit_cast<uint16_t>(content);
  }

  ein_nodicard ein_inline ein_artificial ein_const
  friend constexpr fp16 operator"" _fp16(long double v) noexcept {
    return fp16(static_cast<float>(v));
  }
};


} // namespace ein

namespace std {
  export ein_nodiscard ein_artificial ein_inline ein_const
  constexpr bool isnan(ein::fp16 x) noexcept {
    // Sign doesn't matter, frac not zero (infinity)
    return (x.to_bits() & 0x7FFF) > 0x7c00;
  }

  export template <>
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
} // namespace std
