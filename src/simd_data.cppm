module;

#include <immintrin.h>

using namespace std;

export module ein.simd_data;

import ein.numerics;
import ein.types;

namespace ein {

/// \brief largest simd register width supported on this platform in bytes
export constexpr size_t max_simd_size
#ifdef __AVX512F__
  = 64;
#else
  = 32;
#endif

/// \brief \ref ein::simd_data_t<\p T,\p N> is defined
export template <typename T, size_t N>
concept has_simd_type =
     std::is_pod_v<T>
  && one_of<sizeof(T),1,2,4,8>
  && one_of<sizeof(T)*N,16,32,64>
  && sizeof(T)*N <= max_simd_size;

// \brief unadulterated clang/gcc vector extension type
export template <typename T, size_t N>
requires has_simd_type<T,N>
using simd_data_t = T __attribute__((__vector_size__(N*sizeof(T)),__aligned__(N*sizeof(T))));

/// \brief can we convert simd_data_t<U,N> -> simd_data_t<T,N> automatically using gcc vector extensions?
export template <typename U, typename T, size_t N>
concept has_builtin_convertvector
      = has_simd_type<U,N>
     && has_simd_type<T,N>
     && requires (simd_data_t<U,N> x) {
          __builtin_convertvector(x,simd_data_t<T,N>);
        };

/// \brief is this type one of the types that is handed well automatically by clang/gcc vector extensions?
export template <typename T>
concept simd_builtin = one_of_t<T,int8_t,uint8_t,int16_t,uint16_t,int32_t,uint32_t,float,double>;

/// \cond
template <size_t N> struct si {};
template <size_t N> struct ps {};
template <size_t N> struct pd {};

template <> struct si<128> { using type = __m128i; };
template <> struct ps<128> { using type = __m128; };
template <> struct pd<128> { using type = __m128d; };

template <> struct si<256> { using type = __m256i; };
template <> struct ps<256> { using type = __m256; };
template <> struct pd<256> { using type = __m256d; };

#ifdef __AVX512F__
template <> struct si<512> { using type = __m512i; };
template <> struct ps<512> { using type = __m512; };
template <> struct pd<512> { using type = __m512d; };
#endif
/// \endcond

/// \cond
template <typename T, size_t N> struct simd_intrinsic {};

template <size_t N>
struct simd_intrinsic<float,N> {
  using type = typename ps<N*sizeof(float)*8>::type;
};

template <size_t N>
struct simd_intrinsic<double,N> {
  using type = typename pd<N*sizeof(double)*8>::type;
};

template <not_one_of_t<float,double> T,size_t N>
struct simd_intrinsic<T,N> {
  using type = typename ps<N*sizeof(T)*8>::type;
};
/// \endcond

/// \brief Returns the Intel intrinsic type associated with a simd register full of \p N values of type \p T.
/// \details this can differ from the preferred type used by clang/gcc vector extensions.
/// \hideinitializer
export template <typename T, size_t N>
requires has_simd_type<T,N>
using simd_intrinsic_t = typename simd_intrinsic<T,N>::type;

/// \brief Do we want to use AVX512's notion of an _mmask8, _mmask16, _mmask32, or _mmask64 for masking operations
// involving \p N values of type \p T at a time. Currently I prefer to use it if available, and if the `_mmaskN` type
// matches the register size.
export template <typename T, size_t N>
concept has_mmask
#if __AVX512F__
      = has_simd_type<T,N> && N >= 8;
#else
      = false;
#endif

#ifdef __AVX512F__
/// \cond
template <size_t N> struct mmask {};
template <> struct mmask<8>  { using type = __mmask8; };
template <> struct mmask<16> { using type = __mmask16; };
template <> struct mmask<32> { using type = __mmask32; };
template <> struct mmask<64> { using type = __mmask64; };
/// \endcond

/// If AVX512 is enabled returns the type of an n-bit mmask.
/// \hideinitializer
export template <size_t N>
requires one_of<N,8,16,32,64>
using mmask_t
  = typename mmask<N>::type;
#endif

/// What type of mask should I use?
/// \hideinitializer
export template <typename T, size_t N>
requires has_simd_type<T,N>
using simd_mask_t =
#ifdef __AVX512F__
  std::conditional_t<one_of<N,8,16,32,64>, mmask_t<N>, simd_intrinsic_t<T,N>>;
#else
  simd_intrinsic_t<T,N>;
#endif


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
constexpr auto cmpint(T a, T b) noexcept {
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
constexpr auto cmp(T a, T b) noexcept {
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

} // namespace ein
