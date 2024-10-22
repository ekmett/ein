export module ein.numerics;

import ein.types;

namespace ein::numerics {

export template <auto N, auto ... candidates>
concept one_of = ((N==candidates) || ... || false);

export template <auto N, auto ... candidates>
concept not_one_of = (!one_of<N,candidates...>);

export template <size_t N> struct integer_traits {};
template <> struct integer_traits<8>  { using signed_t = int8_t;  using unsigned_t = uint8_t; };
template <> struct integer_traits<16> { using signed_t = int16_t; using unsigned_t = uint16_t; };
template <> struct integer_traits<32> { using signed_t = int32_t; using unsigned_t = uint32_t; };
template <> struct integer_traits<64> { using signed_t = int64_t; using unsigned_t = uint64_t; };

export template <typename T>
requires one_of<sizeof(T),1,2,4,8>
using int_t = typename integer_traits<sizeof(T)*8>::signed_t;

export template <typename T>
requires one_of<sizeof(T),1,2,4,8>
using uint_t = typename integer_traits<sizeof(T)*8>::unsigned_t;

// allow passing immediate values to operators, without having to give up << n
// as a syntactic form. xs << imm<n> isn't much of an imposition.
export template <size_t N>
struct imm_t {
  constexpr imm_t() noexcept = default;
  constexpr imm_t(imm_t const &) noexcept = default;
  constexpr imm_t(imm_t &&) noexcept = default;
  constexpr imm_t & operator = (imm_t const &) noexcept = default;
  constexpr imm_t & operator = (imm_t &&) noexcept = default;
  constexpr operator size_t () noexcept { return N; }
};

export template <size_t N>
constexpr const imm_t<N> imm {};

// std::isnan is not constexpr until c++23 (todo: retire this)

export EIN(inline) constexpr bool isnan(auto x) noexcept {
  return x/=x;
}

// std::isinf is not constexpr until c++23 (todo: retire this)
static EIN(inline) constexpr bool isinf(auto x) noexcept {
  using T = std::decay_t<decltype(x)>;
  if consteval {
    return x == std::numeric_limits<T>::infinity() || x == -std::numeric_limits<T>::infinity();
  } else {
    return std::isinf(x);
  }
}

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

} // namespace ein::numerics

export namespace ein {
  using namespace ein::numerics;
}
