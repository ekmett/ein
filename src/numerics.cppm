export module ein.numerics;

import ein.types;

namespace ein::numerics {

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
export template <typename T>
requires one_of<sizeof(T),1,2,4,8>
using int_t = typename integer_traits<sizeof(T)*8>::signed_t;

/// returns an unsigned integer type of the same size as \p T suitable for `std::bitcast`
export template <typename T>
requires one_of<sizeof(T),1,2,4,8>
using uint_t = typename integer_traits<sizeof(T)*8>::unsigned_t;

/// \brief A compile time constant passed as an empty struct
/// \details Allow passing immediate values to operators without having to give up `x << y`
/// as a syntactic form. `xs << imm<n>` isn't much of an imposition and ensures the compiler
/// knows that n is a fixed constant known at compile time
export template <size_t N>
struct imm_t {
  constexpr operator size_t () noexcept { return N; }
};

export template <size_t N>
constexpr const imm_t<N> imm {};

/*
export EIN(inline) constexpr bool isnan(auto x) noexcept {
  return x/=x;
}

// std::isinf is not constexpr until c++23 (todo: retire this)
export EIN(inline) constexpr bool isinf(auto x) noexcept {
  using T = std::decay_t<decltype(x)>;
  if consteval {
    return x == std::numeric_limits<T>::infinity() || x == -std::numeric_limits<T>::infinity();
  } else {
    return std::isinf(x);
  }
}
*/

export inline constexpr bool cmp_unord(auto a, auto b) noexcept {
  return is_nan(a) || is_nan(b);
}

export inline constexpr bool cmp_ord(auto a, auto b) noexcept {
  return !is_nan(a) && !is_nan(b);
}


template <one_of_t<float,double> T>
constexpr T scalef(T x, T y) noexcept {
  if consteval {
    // Constexpr path using bit manipulation

    using uint_type = uint_t<T>; //  std::conditional_t<std::is_same_v<T, float>, uint32_t, uint64_t>;
    constexpr int exponent_bias = std::is_same_v<T, float> ? 127 : 1023;
    constexpr uint_type exponent_mask = std::is_same_v<T, float> ? 0x7F800000 : 0x7FF0000000000000ull;
    constexpr uint_type mantissa_mask = std::is_same_v<T, float> ? 0x007FFFFF : 0x000FFFFFFFFFFFFFull;

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

export enum class CMPINT : size_t {
  EQ    = 0x0uz
, LT    = 0x1uz
, LE    = 0x2uz
, FALSE = 0x3uz
, NE    = 0x4uz
, NLT   = 0x5uz
, NLE   = 0x6uz
, TRUE  = 0x7uz
};

export template <CMPINT imm8, typename T>
requires (one_of_t<T,uint8_t,int8_t,uint16_t,int16_t,uint32_t,int32_t,uint64_t,int64_t> && (size_t(imm8) < 8uz))
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

/// AVX512 added many more floating point comparison types. Do we have them?
/// \hideinitializer
export constexpr size_t max_fp_comparison_predicate
#ifdef AVX512
 = 32;
#else
 = 8;
#endif

export enum CMP : size_t {
  EQ_OQ     = 0x00uz  // Equal (ordered, nonsignaling)
, LT_OS     = 0x01uz  // Less-than (ordered, signaling)
, LE_OS     = 0x02uz  // Less-than-or-equal (ordered, signaling)
, UNORD_Q   = 0x03uz  // Unordered (nonsignaling)
, NEQ_UQ    = 0x04uz  // Not-equal (unordered, nonsignaling)
, NLT_US    = 0x05uz  // Not-less-than (unordered, signaling)
, NLE_US    = 0x06uz  // Not-less-than-or-equal (unordered, signaling)
, ORD_Q     = 0x07uz  // Ordered (nonsignaling)
#ifdef __AVX512F__
, EQ_UQ     = 0x08uz  // Equal (unordered, nonsignaling)
, NGE_US    = 0x09uz  // Not-greater-than-or-equal (unordered, signaling)
, NGT_US    = 0x0Auz  // Not-greater-than (unordered, signaling)
, FALSE_OQ  = 0x0Buz  // False (ordered, nonsignaling)
, NEQ_OQ    = 0x0Cuz  // Not-equal (ordered, nonsignaling)
, GE_OS     = 0x0Duz  // Greater-than-or-equal (ordered, signaling)
, GT_OS     = 0x0Euz  // Greater-than (ordered, signaling)
, TRUE_UQ   = 0x0Fuz  // True (unordered, nonsignaling)
, EQ_OS     = 0x10uz  // Equal (ordered, signaling)
, LT_OQ     = 0x11uz  // Less-than (ordered, nonsignaling)
, LE_OQ     = 0x12uz  // Less-than-or-equal (ordered, nonsignaling)
, UNORD_S   = 0x13uz  // Unordered (signaling)
, NEQ_US    = 0x14uz  // Not-equal (unordered, signaling)
, NLT_UQ    = 0x15uz  // Not-less-than (unordered, nonsignaling)
, NLE_UQ    = 0x16uz  // Not-less-than-or-equal (unordered, nonsignaling)
, ORD_S     = 0x17uz  // Ordered (signaling)
, EQ_US     = 0x18uz  // Equal (unordered, signaling)
, NGE_UQ    = 0x19uz  // Not-greater-than-or-equal (unordered, nonsignaling)
, NGT_UQ    = 0x1Auz  // Not-greater-than (unordered, nonsignaling)
, FALSE_OS  = 0x1Buz  // False (ordered, signaling)
, NEQ_OS    = 0x1Cuz  // Not-equal (ordered, signaling)
, GE_OQ     = 0x1Duz  // Greater-than-or-equal (ordered, nonsignaling)
, GT_OQ     = 0x1Euz  // Greater-than (ordered, nonsignaling)
, TRUE_US   = 0x1Fuz  // True (unordered, signaling)
#endif
};

/// perform an avx512 style floating point comparison for scalar values.
export template <CMP imm8, typename T>
requires (one_of_t<T,float,double> && (imm8 < max_fp_comparison_predicate))
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

} // namespace ein::numerics

export namespace ein {
  using namespace ein::numerics;
}
