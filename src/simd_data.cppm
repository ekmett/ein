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

export __m128 cast_ps(__m128i a) noexcept { return _mm_castsi128_ps(a); }
export __m256 cast_ps(__m256i a) noexcept { return _mm256_castsi256_ps(a); }
export __m128d cast_pd(__m128i a) noexcept { return _mm_castsi128_pd(a); }
export __m256d cast_pd(__m256i a) noexcept { return _mm256_castsi256_pd(a); }

export __m128i cast_si(__m128 a) noexcept { return _mm_castps_si128(a); }
export __m256i cast_si(__m256 a) noexcept { return _mm256_castps_si256(a); }
export __m128i cast_si(__m128d a) noexcept { return _mm_castpd_si128(a); }
export __m256i cast_si(__m256d a) noexcept { return _mm256_castpd_si256(a); }
#ifdef __AVX512F__
export __m512 cast_ps(__m512i a) noexcept { return _mm512_castsi512_ps(a); }
export __m512d cast_pd(__m512i a) noexcept { return _mm512_castsi512_pd(a); }
export __m512i cast_si(__m512 a) noexcept { return _mm512_castps_si512(a); }
export __m512i cast_si(__m512d a) noexcept { return _mm512_castpd_si512(a); }
#endif

} // namespace ein
