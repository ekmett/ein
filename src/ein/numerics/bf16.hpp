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
#include "ein/attributes.hpp"

namespace ein {

struct bf16 {
  using underlying_type = __bf16;

  __bf16 content;

  ein_inline ein_artificial constexpr
  bf16() noexcept = default;

  ein_inline ein_artificial constexpr
  bf16(float f) noexcept : content(f) {}

  ein_inline ein_artificial constexpr
  bf16(__bf16 f) noexcept : content(f) {}

  ein_inline ein_artificial constexpr
  bf16(ein_noescape bf16 const &) noexcept = default;

  ein_inline ein_artificial constexpr
  bf16(ein_noescape bf16 &&) noexcept = default;

  ein_nodiscard ein_inline ein_artificial ein_const constexpr
  operator __bf16 (this bf16 self) noexcept { return self.content; }

  ein_nodiscard ein_inline ein_artificial ein_const constexpr
  operator float (this bf16 self) noexcept { return self.content; }

  ein_reinitializes ein_inline ein_artificial constexpr
  bf16 & operator = (ein_noescape bf16 const &) noexcept
  ein_lifetimebound = default;

  ein_reinitializes ein_inline ein_artificial constexpr
  bf16 & operator = (ein_noescape bf16 &&) noexcept
  ein_lifetimebound = default;

  ein_nodiscard ein_inline ein_artificial ein_const
  friend constexpr bool operator == (bf16 x, bf16 y) noexcept {
    return x.content == y.content;
  }

  ein_nodiscard ein_inline ein_artificial ein_const
  friend constexpr std::partial_ordering operator <=> (bf16 x, bf16 y) noexcept {
    return x.content <=> y.content;
  }

  ein_nodiscard ein_inline ein_artificial ein_const
  friend constexpr bool operator != (bf16 x, bf16 y) noexcept {
    return x.content != y.content;
  }

  ein_nodiscard ein_inline ein_artificial ein_const
  friend constexpr bool operator < (bf16 x, bf16 y) noexcept {
    return x.content < y.content;
  }

  ein_nodiscard ein_inline ein_artificial ein_const
  friend constexpr bool operator > (bf16 x, bf16 y) noexcept {
    return x.content > y.content;
  }

  ein_nodiscard ein_inline ein_artificial ein_const
  friend constexpr bool operator <= (bf16 x, bf16 y) noexcept {
    return x.content <= y.content;
  }

  ein_nodiscard ein_inline ein_artificial ein_const
  friend constexpr bool operator >= (bf16 x, bf16 y) noexcept {
    return x.content >= y.content;
  }

  ein_inline ein_artificial
  friend constexpr void swap(
    ein_noescape bf16 & x,
    ein_noescape bf16 & y
  ) noexcept {
    using std::swap;
    swap(x.content,y.content);
  }

  ein_nodiscard ein_inline ein_artificial ein_const
  static constexpr bf16 from_bits(uint16_t data) noexcept {
    return std::bit_cast<bf16>(data);
  }

  ein_nodiscard ein_inline ein_const
  constexpr uint16_t to_bits(this bf16 self) noexcept {
    return std::bit_cast<uint16_t>(self.content);
  }
};

ein_nodiscard ein_inline ein_artificial
constexpr bf16 operator""_bf16(long double v) noexcept {
  return bf16(static_cast<float>(v));
}

ein_nodiscard ein_inline ein_artificial ein_const
constexpr bf16 fast_to_bf16(float f) noexcept {
  if consteval {
    return f;
  } else {
    return std::bit_cast<bf16>(static_cast<uint16_t>(std::bit_cast<uint32_t>(f)>>16));
  }
}

ein_nodiscard ein_inline ein_artificial ein_const
constexpr bf16 fast_to_bf16(bf16 f) noexcept {
  return f;
}

} // namespace ein

namespace std {
  ein_nodiscard ein_artificial ein_inline ein_const
  constexpr bool isnan(ein::bf16 x) noexcept {
    // Exponent maxed out and fraction non-zero indicates NaN
    return (x.to_bits() & 0x7FFF) > 0x7F80;
  }

  template <> class numeric_limits<ein::bf16> {
  public:
    static constexpr bool is_specialized = true;
    ein_nodiscard ein_inline ein_artificial ein_const
    static constexpr ein::bf16 min() noexcept {
      return ein::bf16::from_bits(0x007F);
    }
    ein_nodiscard ein_inline ein_artificial ein_const
    static constexpr ein::bf16 max() noexcept {
      return ein::bf16::from_bits(0x7F7F);
    }
    ein_nodiscard ein_inline ein_artificial ein_const
    static constexpr ein::bf16 lowest() noexcept {
      return ein::bf16::from_bits(0xFF7F);
    }
    static constexpr int digits = 7;
    static constexpr int digits10 = 2;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr int radix = 2;
    ein_nodiscard ein_inline ein_artificial ein_const
    static constexpr ein::bf16 epsilon() noexcept {
      return ein::bf16::from_bits(0x3C00);
    }
    ein_nodiscard ein_inline ein_artificial ein_const
    static constexpr ein::bf16 round_error() noexcept {
      return ein::bf16::from_bits(0x3F00);
    }
    static constexpr int min_exponent = -125;
    static constexpr int min_exponent10 = -37;
    static constexpr int max_exponent = 128;
    static constexpr int max_exponent10 = 38;
    static constexpr bool has_infinity = true;
    static constexpr bool has_quiet_NaN = true;
    static constexpr bool has_signaling_NaN = true;
    static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;
    ein_nodiscard ein_inline ein_artificial ein_const
    static constexpr ein::bf16 infinity() noexcept {
      return ein::bf16::from_bits(0x7F80);
    }
    ein_nodiscard ein_inline ein_artificial ein_const
    static constexpr ein::bf16 quiet_NaN() noexcept {
      return ein::bf16::from_bits(0x7FC0);
    }
    ein_nodiscard ein_inline ein_artificial ein_const
    static constexpr ein::bf16 signaling_NaN() noexcept {
      return ein::bf16::from_bits(0x7FC0);
    }
    ein_nodiscard ein_inline ein_artificial ein_const
    static constexpr ein::bf16 denorm_min() noexcept {
      return ein::bf16{};
    }
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = false;
    static constexpr bool is_modulo = false;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_to_nearest;
  };
} // namespace std