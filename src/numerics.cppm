/// \file
/// \ingroup numerics
/// \license
/// SPDX-FileType: Source
/// SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
/// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
/// \endlicense

/// \ingroup numerics
export module ein.numerics;

import ein.types;

using namespace std;

namespace ein {
/// \defgroup numerics numerics
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

/// \brief A compile time constant passed as an empty struct
/// \details Allow passing immediate values to operators without having to give up `x << y`
/// as a syntactic form. `xs << imm<n>` isn't much of an imposition and ensures the compiler
/// knows that n is a fixed constant known at compile time
export template <size_t N>
struct imm_t {
  static constexpr size_t value = N;
  /// \nodiscard \inline \const \artificial \cond
  EIN(nodiscard,inline,const,artificial) /// \endcond
  constexpr operator size_t () noexcept { return N; }
};

export template <size_t N>
constinit imm_t<N> imm {};

/// \nodiscard \inline \pure
export template <typename T>   /// \cond
[[nodiscard]] EIN(inline,pure) /// \endcond
constexpr bool cmp_unord(T a, T b) noexcept {
  return isnan(a) || isnan(b);
}

export template bool cmp_unord(float,float) noexcept;
export template bool cmp_unord(double,double) noexcept;

/// \nodiscard \inline \pure
export template <typename T>   /// \cond
[[nodiscard]] EIN(inline,pure) /// \endcond
constexpr bool cmp_ord(T a, T b) noexcept {
  return !isnan(a) && !isnan(b);
}

export template bool cmp_ord(float,float) noexcept;
export template bool cmp_ord(double,double) noexcept;

/// \hideinlinesource \nodiscard \inline \pure
export template <one_of_t<float,double> T> /// \cond
[[nodiscard]] EIN(inline,pure) /// \endcond
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

// compile both ways
export template float scalef(float, float) noexcept;
export template double scalef(double, double) noexcept;

/// \hideinlinesource
export enum class [[nodiscard]] CMPINT : size_t {
  EQ    = 0x0uz ///< `==`
, LT    = 0x1uz ///< `<`
, LE    = 0x2uz ///< `<=`
, FALSE = 0x3uz ///< always `false`
, NE    = 0x4uz ///< `!=`
, NLT   = 0x5uz ///< `>=`
, NLE   = 0x6uz ///< `>`
, TRUE  = 0x7uz ///< always `true`
};

/// \nodiscard \inline \const
export template <CMPINT imm8, typename T>
requires (one_of_t<T,uint8_t,int8_t,uint16_t,int16_t,uint32_t,int32_t,uint64_t,int64_t> && (size_t(imm8) < 8uz)) /// \cond
[[nodiscard]] EIN(inline,const) /// \endcond
constexpr bool cmpint(T a, T b) noexcept {
  if      constexpr (imm8 == CMPINT::TRUE)  return -1;
  else if constexpr (imm8 == CMPINT::FALSE) return 0;
  else if constexpr (imm8 == CMPINT::LT)    return a < b;
  else if constexpr (imm8 == CMPINT::NLT)   return a >= b;
  else if constexpr (imm8 == CMPINT::LE)    return a <= b;
  else if constexpr (imm8 == CMPINT::NLE)   return a > b;
  else if constexpr (imm8 == CMPINT::EQ)    return a == b;
  else if constexpr (imm8 == CMPINT::NE)    return a != b;
  else static_assert(false);
}

// precompile cmpint

/// \cond
#define EIN_CMPINT_IMPL(X,Y) \
  export template bool cmpint<CMPINT::X,Y>(Y,Y) noexcept;
#define EIN_CMPINT(X) \
  EIN_CMPINT_IMPL(X,uint8_t) \
  EIN_CMPINT_IMPL(X,uint16_t) \
  EIN_CMPINT_IMPL(X,uint32_t) \
  EIN_CMPINT_IMPL(X,uint64_t) \
  EIN_CMPINT_IMPL(X,int8_t) \
  EIN_CMPINT_IMPL(X,int16_t) \
  EIN_CMPINT_IMPL(X,int32_t) \
  EIN_CMPINT_IMPL(X,int64_t)
/// \endcond

EIN_CMPINT(TRUE)
EIN_CMPINT(FALSE)
EIN_CMPINT(LT)
EIN_CMPINT(NLT)
EIN_CMPINT(LE)
EIN_CMPINT(NLE)
EIN_CMPINT(EQ)
EIN_CMPINT(NE)

#undef EIN_CMPINT_IMPL
#undef EIN_CMPINT

/// AVX512 added many more floating point comparison types. Do we have them?
/// \hideinitializer
export constexpr size_t max_fp_comparison_predicate
#ifdef AVX512
 = 32;
#else
 = 8;
#endif

/// \hideinlinesource
export enum class [[nodiscard]] CMP : size_t {
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
/// \nodiscard \pure \inline
export template <CMP imm8, typename T>
requires (one_of_t<T,float,double> && (size_t(imm8) < 32uz)) /// \cond
[[nodiscard]] EIN(pure,inline) /// \endcond
constexpr bool cmp(T a, T b) noexcept {
  if      constexpr (imm8 == CMP::EQ_OQ)    return cmp_ord(a, b) && (a == b);
  else if constexpr (imm8 == CMP::LT_OS)    return cmp_ord(a, b) && (a < b);
  else if constexpr (imm8 == CMP::LE_OS)    return cmp_ord(a, b) && (a <= b);
  else if constexpr (imm8 == CMP::UNORD_Q)  return cmp_unord(a, b);
  else if constexpr (imm8 == CMP::NEQ_UQ)   return cmp_unord(a, b) || (a != b);
  else if constexpr (imm8 == CMP::NLT_US)   return cmp_unord(a, b) || !(a < b);
  else if constexpr (imm8 == CMP::NLE_US)   return cmp_unord(a, b) || !(a <= b);
  else if constexpr (imm8 == CMP::ORD_Q)    return cmp_ord(a, b);
  else if constexpr (imm8 == CMP::EQ_UQ)    return cmp_unord(a, b) || (a == b);
  else if constexpr (imm8 == CMP::NGE_US)   return cmp_unord(a, b) || !(a >= b);
  else if constexpr (imm8 == CMP::NGT_US)   return cmp_unord(a, b) || !(a > b);
  else if constexpr (imm8 == CMP::FALSE_OQ) return 0;
  else if constexpr (imm8 == CMP::NEQ_OQ)   return cmp_ord(a, b) && (a != b);
  else if constexpr (imm8 == CMP::GE_OS)    return cmp_ord(a, b) && (a >= b);
  else if constexpr (imm8 == CMP::GT_OS)    return cmp_ord(a, b) && (a > b);
  else if constexpr (imm8 == CMP::TRUE_UQ)  return -1;
  else if constexpr (imm8 == CMP::EQ_OS)    return cmp_ord(a, b) && (a == b);
  else if constexpr (imm8 == CMP::LT_OQ)    return cmp_ord(a, b) && (a < b);
  else if constexpr (imm8 == CMP::LE_OQ)    return cmp_ord(a, b) && (a <= b);
  else if constexpr (imm8 == CMP::UNORD_S)  return cmp_unord(a, b);
  else if constexpr (imm8 == CMP::NEQ_US)   return cmp_unord(a, b) || (a != b);
  else if constexpr (imm8 == CMP::NLT_UQ)   return cmp_unord(a, b) || !(a < b);
  else if constexpr (imm8 == CMP::NLE_UQ)   return cmp_unord(a, b) || !(a <= b);
  else if constexpr (imm8 == CMP::ORD_S)    return cmp_ord(a, b);
  else if constexpr (imm8 == CMP::EQ_US)    return cmp_unord(a, b) || (a == b);
  else if constexpr (imm8 == CMP::NGE_UQ)   return cmp_unord(a, b) || !(a >= b);
  else if constexpr (imm8 == CMP::NGT_UQ)   return cmp_unord(a, b) || !(a > b);
  else if constexpr (imm8 == CMP::FALSE_OS) return 0;
  else if constexpr (imm8 == CMP::NEQ_OS)   return cmp_ord(a, b) && (a != b);
  else if constexpr (imm8 == CMP::GE_OQ)    return cmp_ord(a, b) && (a >= b);
  else if constexpr (imm8 == CMP::GT_OQ)    return cmp_ord(a, b) && (a > b);
  else if constexpr (imm8 == CMP::TRUE_US)  return -1;
  else static_assert(false);
}

/// \cond
#define EIN_CMP_IMPL(X,Y) \
  export template bool cmp<CMP::X,Y>(Y,Y) noexcept;
#define EIN_CMP(X) \
  EIN_CMP_IMPL(X,float) \
  EIN_CMP_IMPL(X,double)
/// \endcond

EIN_CMP(EQ_OQ)
EIN_CMP(LT_OS)
EIN_CMP(LE_OS)
EIN_CMP(UNORD_Q)
EIN_CMP(NEQ_UQ)
EIN_CMP(NLT_US)
EIN_CMP(NLE_US)
EIN_CMP(ORD_Q)
EIN_CMP(EQ_UQ)
EIN_CMP(NGE_US)
EIN_CMP(NGT_US)
EIN_CMP(FALSE_OQ)
EIN_CMP(NEQ_OQ)
EIN_CMP(GE_OS)
EIN_CMP(GT_OS)
EIN_CMP(TRUE_UQ)
EIN_CMP(EQ_OS)
EIN_CMP(LT_OQ)
EIN_CMP(LE_OQ)
EIN_CMP(UNORD_S)
EIN_CMP(NEQ_US)
EIN_CMP(NLT_UQ)
EIN_CMP(NLE_UQ)
EIN_CMP(ORD_S)
EIN_CMP(EQ_US)
EIN_CMP(NGE_UQ)
EIN_CMP(NGT_UQ)
EIN_CMP(FALSE_OS)
EIN_CMP(NEQ_OS)
EIN_CMP(GE_OQ)
EIN_CMP(GT_OQ)
EIN_CMP(TRUE_US)

#undef EIN_CMP
#undef EIN_CMP_IMPL

/// \}
}
