/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \ingroup numerics */

module;

#ifdef EIN_PRELUDE
#include "prelude.hpp"
#elifndef EIN_PCH
#include <bit>
#include <cmath>
#include <cstdint>
#include <type_traits>
#include "attributes.hpp"
#endif

#ifdef __AVX512F__
#define IFAVX512(x,y) x
#else
#define IFAVX512(x,y) y
#endif

using namespace std;

import ein.types;
import ein.bf16;
//import ein.fp16;

/// \ingroup numerics
export module ein.numerics;

namespace ein {
/// \defgroup numerics Numerics
/// \{

/// \brief \p N is one of the \p candidates
export template <auto N, auto ... candidates>
concept one_of = ((N==candidates) || ... || false);

/// \brief \p N is not one of the \p candidates
export template <auto N, auto ... candidates>
concept not_one_of = (!one_of<N,candidates...>);

/// \cond
export template <size_t N> struct integer_traits {};
template <> struct integer_traits<8>  { using signed_t = int8_t;  using unsigned_t = uint8_t; };
template <> struct integer_traits<16> { using signed_t = int16_t; using unsigned_t = uint16_t; };
template <> struct integer_traits<32> { using signed_t = int32_t; using unsigned_t = uint32_t; };
template <> struct integer_traits<64> { using signed_t = int64_t; using unsigned_t = uint64_t; };
/// \endcond

/// returns a signed integer type of the same size as \p T suitable for `std::bitcast`
/// \hideinitializer \hideinlinesource
export template <typename T>
requires one_of<sizeof(T),1,2,4,8>
using int_t = typename integer_traits<sizeof(T)*8>::signed_t;

/// returns an unsigned integer type of the same size as \p T suitable for `std::bitcast`
/// \hideinitializer \hideinlinesource
export template <typename T>
requires one_of<sizeof(T),1,2,4,8>
using uint_t = typename integer_traits<sizeof(T)*8>::unsigned_t;

/** \brief A compile time constant passed as an empty struct

    \details

    Allow passing immediate values to operators without having to give up `x << y`
    as a syntactic form. `xs << imm<n>` isn't much of an imposition and ensures the compiler
    knows that \p N is a fixed constant known at compile time */
export template <size_t N>
struct imm_t {
  static constexpr size_t value = N;
  ein_nodiscard ein_inline ein_const ein_artificial
  constexpr operator size_t () noexcept { return N; }
};

export template <size_t N>
constinit imm_t<N> imm {};

export template <typename T>
ein_nodiscard ein_inline ein_pure
constexpr bool cmp_unord(T a, T b) noexcept {
  return isnan(a) || isnan(b);
}

/// \cond precompile
export extern template bool cmp_unord(float,float) noexcept;
export extern template bool cmp_unord(double,double) noexcept;
/// \endcond

export template <typename T>
ein_nodiscard ein_inline ein_pure
constexpr bool cmp_ord(T a, T b) noexcept {
  return !isnan(a) && !isnan(b);
}

/// \cond precompile
export extern template bool cmp_ord(float,float) noexcept;
export extern template bool cmp_ord(double,double) noexcept;
/// \endcond

/// \hideinlinesource
export template <one_of_t<float,double> T>
ein_nodiscard ein_inline ein_pure
constexpr T scalef(T x, T y) noexcept {
  if consteval {
    // Constexpr path using bit manipulation

    using uint_type = uint_t<T>; //  std::conditional_t<std::is_same_v<T, float>, uint32_t, uint64_t>;
    constexpr int exponent_bias = std::is_same_v<T, float> ? 127 : 1023;
    constexpr uint_type exponent_mask = std::is_same_v<T, float> ? uint_type(0x7F800000) : uint_type(0x7FF0000000000000ull);
    constexpr uint_type mantissa_mask = std::is_same_v<T, float> ? uint_type(0x007FFFFF) : uint_type(0x000FFFFFFFFFFFFFull);

    if (x == 0.0 || isnan(x) || std::isinf(x))
      return x; // Handle special cases

    // Get the raw bits of the floating-point number
    uint_type x_bits = std::bit_cast<uint_type>(x);

    // Extract exponent and mantissa
    int exponent = static_cast<int>(((x_bits & exponent_mask) >> (std::is_same_v<T, float> ? 23 : 52)) - exponent_bias);
    uint_type mantissa = x_bits & mantissa_mask;

    // Normalize the mantissa (implicit leading 1 for normalized numbers)
    if (exponent != -exponent_bias)
      mantissa |= std::is_same_v<T, float> ? (1u << 23) : (1ull << 52); // Set implicit leading bit

    // Scale the exponent by adding y
    exponent += static_cast<int>(y);

    // Check for overflow and underflow
    if (exponent > std::numeric_limits<T>::max_exponent)
      return std::numeric_limits<T>::infinity();

    if (exponent < std::numeric_limits<T>::min_exponent)
      return static_cast<T>(0.0); // Underflow to zero

    // Rebuild the floating-point number from the new exponent and mantissa
    x_bits =
      (x_bits & (std::is_same_v<T, float> ? 0x80000000u : 0x8000000000000000ull)) | // Preserve the sign bit
      (static_cast<uint_type>(exponent + exponent_bias) << (std::is_same_v<T, float> ? 23 : 52)) | // Apply new exponent
      (mantissa & mantissa_mask); // Mask mantissa bits

    return std::bit_cast<T>(x_bits);
  } else {
    // Runtime path using fast standard library call
    return std::scalbn(x, static_cast<int>(y));
  }
}

/// \hideinlinesource
export enum class ein_nodiscard CMPINT : size_t {
  EQ    = 0x0uz ///< `==`
, LT    = 0x1uz ///< `<`
, LE    = 0x2uz ///< `<=`
, FALSE = 0x3uz ///< always `false`
, NE    = 0x4uz ///< `!=`
, NLT   = 0x5uz ///< `>=`
, NLE   = 0x6uz ///< `>`
, TRUE  = 0x7uz ///< always `true`
};

export template <CMPINT imm8, typename T>
requires (one_of_t<T,uint8_t,int8_t,uint16_t,int16_t,uint32_t,int32_t,uint64_t,int64_t> && (size_t(imm8) < 8uz))
ein_nodiscard ein_inline ein_const
constexpr bool cmpint(T a, T b) noexcept {
  using enum CMPINT;
  if      constexpr (imm8 == TRUE)  return -1;
  else if constexpr (imm8 == FALSE) return 0;
  else if constexpr (imm8 == LT)    return a < b;
  else if constexpr (imm8 == NLT)   return a >= b;
  else if constexpr (imm8 == LE)    return a <= b;
  else if constexpr (imm8 == NLE)   return a > b;
  else if constexpr (imm8 == EQ)    return a == b;
  else if constexpr (imm8 == NE)    return a != b;
  else static_assert(false);
}

/// AVX512 added many more floating point comparison types. Do we have them?
/// \hideinitializer
export constexpr size_t max_fp_comparison_predicate = IFAVX512(32,8);

/// \hideinlinesource
export enum class ein_nodiscard CMP : size_t {
  EQ_OQ     = 0x00uz  ///< Equal (ordered, nonsignaling)
, LT_OS     = 0x01uz  ///< Less-than (ordered, signaling)
, LE_OS     = 0x02uz  ///< Less-than-or-equal (ordered, signaling)
, UNORD_Q   = 0x03uz  ///< Unordered (nonsignaling)
, NEQ_UQ    = 0x04uz  ///< Not-equal (unordered, nonsignaling)
, NLT_US    = 0x05uz  ///< Not-less-than (unordered, signaling)
, NLE_US    = 0x06uz  ///< Not-less-than-or-equal (unordered, signaling)
, ORD_Q     = 0x07uz  ///< Ordered (nonsignaling)
, EQ_UQ     = 0x08uz  ///< Equal (unordered, nonsignaling) (AVX-512)
, NGE_US    = 0x09uz  ///< Not-greater-than-or-equal (unordered, signaling) (AVX-512)
, NGT_US    = 0x0Auz  ///< Not-greater-than (unordered, signaling) (AVX-512)
, FALSE_OQ  = 0x0Buz  ///< False (ordered, nonsignaling) (AVX-512)
, NEQ_OQ    = 0x0Cuz  ///< Not-equal (ordered, nonsignaling) (AVX-512)
, GE_OS     = 0x0Duz  ///< Greater-than-or-equal (ordered, signaling) (AVX-512)
, GT_OS     = 0x0Euz  ///< Greater-than (ordered, signaling) (AVX-512)
, TRUE_UQ   = 0x0Fuz  ///< True (unordered, nonsignaling) (AVX-512)
, EQ_OS     = 0x10uz  ///< Equal (ordered, signaling) (AVX-512)
, LT_OQ     = 0x11uz  ///< Less-than (ordered, nonsignaling) (AVX-512)
, LE_OQ     = 0x12uz  ///< Less-than-or-equal (ordered, nonsignaling) (AVX-512)
, UNORD_S   = 0x13uz  ///< Unordered (signaling) (AVX-512)
, NEQ_US    = 0x14uz  ///< Not-equal (unordered, signaling) (AVX-512)
, NLT_UQ    = 0x15uz  ///< Not-less-than (unordered, nonsignaling) (AVX-512)
, NLE_UQ    = 0x16uz  ///< Not-less-than-or-equal (unordered, nonsignaling) (AVX-512)
, ORD_S     = 0x17uz  ///< Ordered (signaling) (AVX-512)
, EQ_US     = 0x18uz  ///< Equal (unordered, signaling) (AVX-512)
, NGE_UQ    = 0x19uz  ///< Not-greater-than-or-equal (unordered, nonsignaling) (AVX-512)
, NGT_UQ    = 0x1Auz  ///< Not-greater-than (unordered, nonsignaling) (AVX-512)
, FALSE_OS  = 0x1Buz  ///< False (ordered, signaling) (AVX-512)
, NEQ_OS    = 0x1Cuz  ///< Not-equal (ordered, signaling) (AVX-512)
, GE_OQ     = 0x1Duz  ///< Greater-than-or-equal (ordered, nonsignaling) (AVX-512)
, GT_OQ     = 0x1Euz  ///< Greater-than (ordered, nonsignaling) (AVX-512)
, TRUE_US   = 0x1Fuz  ///< True (unordered, signaling) (AVX-512)
};

/// perform an avx512 style floating point comparison for scalar values.
export template <CMP imm8, typename T>
requires (one_of_t<T,float,double> && (size_t(imm8) < 32uz))
ein_nodiscard ein_inline ein_pure
constexpr bool cmp(T a, T b) noexcept {
  using enum CMP;
  if      constexpr (imm8 == EQ_OQ)    return cmp_ord(a, b) && (a == b);
  else if constexpr (imm8 == LT_OS)    return cmp_ord(a, b) && (a < b);
  else if constexpr (imm8 == LE_OS)    return cmp_ord(a, b) && (a <= b);
  else if constexpr (imm8 == UNORD_Q)  return cmp_unord(a, b);
  else if constexpr (imm8 == NEQ_UQ)   return cmp_unord(a, b) || (a != b);
  else if constexpr (imm8 == NLT_US)   return cmp_unord(a, b) || !(a < b);
  else if constexpr (imm8 == NLE_US)   return cmp_unord(a, b) || !(a <= b);
  else if constexpr (imm8 == ORD_Q)    return cmp_ord(a, b);
  else if constexpr (imm8 == EQ_UQ)    return cmp_unord(a, b) || (a == b);
  else if constexpr (imm8 == NGE_US)   return cmp_unord(a, b) || !(a >= b);
  else if constexpr (imm8 == NGT_US)   return cmp_unord(a, b) || !(a > b);
  else if constexpr (imm8 == FALSE_OQ) return 0;
  else if constexpr (imm8 == NEQ_OQ)   return cmp_ord(a, b) && (a != b);
  else if constexpr (imm8 == GE_OS)    return cmp_ord(a, b) && (a >= b);
  else if constexpr (imm8 == GT_OS)    return cmp_ord(a, b) && (a > b);
  else if constexpr (imm8 == TRUE_UQ)  return -1;
  else if constexpr (imm8 == EQ_OS)    return cmp_ord(a, b) && (a == b);
  else if constexpr (imm8 == LT_OQ)    return cmp_ord(a, b) && (a < b);
  else if constexpr (imm8 == LE_OQ)    return cmp_ord(a, b) && (a <= b);
  else if constexpr (imm8 == UNORD_S)  return cmp_unord(a, b);
  else if constexpr (imm8 == NEQ_US)   return cmp_unord(a, b) || (a != b);
  else if constexpr (imm8 == NLT_UQ)   return cmp_unord(a, b) || !(a < b);
  else if constexpr (imm8 == NLE_UQ)   return cmp_unord(a, b) || !(a <= b);
  else if constexpr (imm8 == ORD_S)    return cmp_ord(a, b);
  else if constexpr (imm8 == EQ_US)    return cmp_unord(a, b) || (a == b);
  else if constexpr (imm8 == NGE_UQ)   return cmp_unord(a, b) || !(a >= b);
  else if constexpr (imm8 == NGT_UQ)   return cmp_unord(a, b) || !(a > b);
  else if constexpr (imm8 == FALSE_OS) return 0;
  else if constexpr (imm8 == NEQ_OS)   return cmp_ord(a, b) && (a != b);
  else if constexpr (imm8 == GE_OQ)    return cmp_ord(a, b) && (a >= b);
  else if constexpr (imm8 == GT_OQ)    return cmp_ord(a, b) && (a > b);
  else if constexpr (imm8 == TRUE_US)  return -1;
  else static_assert(false);
}

/// \cond precompile
#define X export extern
#include "numerics.x"
#undef X
/// \endcond

/// \}
}
