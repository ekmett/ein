module;

#include <immintrin.h>

using namespace std;

export module ein.simd;

import ein.numerics;
import ein.types;

namespace ein {

export constexpr size_t max_simd_size
#ifdef __AVX512F__
  = 64;
#else
  = 32;
#endif

template <typename T, size_t N>
concept has_simd_type =
     std::is_pod_v<T>
  && one_of<sizeof(T),1,2,4,8>
  && one_of<sizeof(T)*N,16,32,64>
  && sizeof(T)*N <= max_simd_size;

#ifndef DOXYGEN
// clang/gcc vector extension type
template <typename T, size_t N>
requires has_simd_type<T,N>
using simd_data_t = T __attribute__((__vector_size__(N*sizeof(T)),__aligned__(N*sizeof(T))));
#endif

// can convert simd_data_t<U,N> -> simd_data_t<T,N> automatically using gcc vector extensions
template <typename U, typename T, size_t N>
concept has_builtin_convertvector
      = has_simd_type<U,N>
     && has_simd_type<T,N>
     && requires (simd_data_t<U,N> x) {
          __builtin_convertvector(x,simd_data_t<T,N>);
        };

template <typename T>
concept simd_builtin = one_of_t<T,int8_t,uint8_t,int16_t,uint16_t,int32_t,uint32_t,float,double>;

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

template <typename T, size_t N> struct simd_intrinsic {};

template <size_t N>
struct simd_intrinsic<float,N> {
  using type = typename ps<N*sizeof(float)*8>::type;
};

template <size_t N>
struct simd_intrinsic<double,N> {
  using type = typename pd<N*sizeof(double)*8>::type;
};

export template <not_one_of_t<float,double> T,size_t N>
struct simd_intrinsic<T,N> {
  using type = typename ps<N*sizeof(T)*8>::type;
};

export template <typename T, size_t N>
requires has_simd_type<T,N>
using simd_intrinsic_t = typename simd_intrinsic<T,N>::type;

export template <typename T, size_t N>
concept has_mmask
#if __AVX512F__
      = has_simd_type<T,N> && N >= 8;
#else
      = false;
#endif

#ifdef __AVX512F__
template <size_t N> struct mmask {};
template <> struct mmask<8>  { using type = __mmask8; };
template <> struct mmask<16> { using type = __mmask16; };
template <> struct mmask<32> { using type = __mmask32; };
template <> struct mmask<64> { using type = __mmask64; };

export template <size_t N>
requires one_of<N,8,16,32,64>
using mmask_t = typename mmask<N>::type;
#endif

export template <typename T, size_t N>
requires has_simd_type<T,N>
using simd_mask_t =
#ifdef __AVX512F__
  std::conditional_t<one_of<N,8,16,32,64>, mmask_t<N>, simd_intrinsic_t<T,N>>;
#else
  simd_intrinsic_t<T,N>;
#endif

#ifdef AVX512
export constexpr size_t max_fp_comparison_predicate = 32;
#else
export constexpr size_t max_fp_comparison_predicate = 8;
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
#ifdef AVX512
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
#endif
  else static_assert(false);
}

export template <typename T, size_t N>
requires has_simd_type<T,N>
struct simd {
  using data_t = simd_data_t<T,N>;
  using mask_t = simd_mask_t<T,N>;
  using intrinsic_t = simd_intrinsic_t<T,N>;
  inline static const size_t bytesize = N*sizeof(T);

  template <typename U>
  using simd_t = simd<U,N>;

  template <typename U>
  using cast_t = simd<U,N*sizeof(T)/sizeof(U)>;

  using value_type = T;
  inline static const size_t size = N;

  data_t data;

  EIN(inline,artificial)
  constexpr simd() noexcept = default;

  EIN(inline,artificial)
  constexpr simd(simd const &) noexcept = default;

  EIN(inline,artificial)
  constexpr simd(simd &&) noexcept = default;

  template <std::convertible_to<T> ... Args>
  EIN(inline,artificial)
  constexpr simd(Args && ... args) noexcept
  requires (sizeof...(Args) == N)
  : data(std::forward<Args>(args)...) {}

  EIN(inline,artificial)
  constexpr simd(T value) noexcept
  : data(value) {}

  EIN(inline,artificial)
  constexpr simd(data_t const & data) noexcept
  : data(data) {}

  EIN(inline,artificial)
  constexpr simd(data_t && data) noexcept
  : data(std::move(data)) {}

  // constexpr simd(const T (&list)[N]) :data(list) {}

  EIN(inline,artificial)
  constexpr simd(std::initializer_list<T> init) {
    // NB: initializer_lists are janky af
    std::copy(init.begin(),std::min(N,init.size()),begin());
    /// TODO: (masked)loadu(std::data(init))
  }

  // concat
  //constexpr simd(simd<T,N/2> a, simd<T,N/2> b) : data(__builtin_shufflevector(a,b,...,...) {}

  EIN(inline,artificial)
  constexpr simd(intrinsic_t const & data) noexcept
  requires (!std::is_same_v<data_t, intrinsic_t>)
  : data(reinterpret_cast<data_t>(data)) {}

  EIN(inline,artificial)
  constexpr simd(intrinsic_t && data) noexcept
  requires (!std::is_same_v<data_t, intrinsic_t>)
  : data(reinterpret_cast<data_t>(std::move(data))) {}

  EIN(reinitializes,inline,artificial)
  constexpr simd & operator = (simd &&) noexcept = default;
  EIN(reinitializes,inline,artificial)
  constexpr simd & operator = (simd const &) noexcept = default;

  // auto convert
  template <typename U>
  requires (!std::is_same_v<U,T> && has_builtin_convertvector<U,T,N>)
  EIN(reinitializes,inline,artificial)
  constexpr simd & operator = (simd_t<U> other) noexcept {
    if consteval {
      for (int i=0;i<N;++i)
        data[i] = T(other.data[i]);
    } else {
      data = __builtin_convertvector(other.data,data_t);
    }
  }

  // provide compatibility with intel intrinsics
  // these map us to/from __m(128|256|512)(|d|i)
  EIN(inline,artificial,const)
  constexpr operator intrinsic_t & () noexcept {
    if constexpr (std::is_same_v<intrinsic_t,data_t>) return data;
    else return reinterpret_cast<intrinsic_t &>(data);
  }
  EIN(inline,artificial,const)
  constexpr operator intrinsic_t const & () const noexcept {
    if constexpr (std::is_same_v<intrinsic_t,data_t>) return data;
    else return reinterpret_cast<intrinsic_t const &>(data);
  }
  EIN(inline,artificial,const)
  constexpr intrinsic_t & it() noexcept {
    if constexpr (std::is_same_v<intrinsic_t,data_t>) return data;
    else return reinterpret_cast<intrinsic_t &>(data);
  }
  EIN(inline,artificial,const)
  constexpr intrinsic_t const & it() const noexcept {
    if constexpr (std::is_same_v<intrinsic_t,data_t>) return data;
    else return reinterpret_cast<intrinsic_t const &>(data);
  }

  EIN(inline,artificial,const)
  constexpr T & operator[](std::size_t index) noexcept { return data[index]; }
  EIN(inline,artificial,const)
  constexpr T const & operator[](std::size_t index) const noexcept { return data[index]; }

  EIN(inline,artificial,const)
  constexpr T * begin() noexcept { return &data; }
  EIN(inline,artificial,const)
  constexpr T * end() noexcept { return begin() + N; }
  EIN(inline,artificial,const)
  constexpr T const * cbegin() const noexcept { return &data[0]; }
  EIN(inline,artificial,const)
  constexpr T const * cend() const noexcept { return begin() + N; }
  EIN(inline,artificial,const)
  constexpr T const * begin() const noexcept { return cbegin(); }
  EIN(inline,artificial,const)
  constexpr T const * end() const noexcept { return cend(); }

  EIN(inline,artificial,const)
  constexpr std::reverse_iterator<T*> rbegin() noexcept { return std::reverse_iterator<T*>(end()); }
  EIN(inline,artificial,const)
  constexpr std::reverse_iterator<T*> rend() noexcept { return std::reverse_iterator<T*>(begin()); }

  EIN(inline,artificial,const)
  constexpr std::reverse_iterator<const T*> crbegin() const noexcept { return std::reverse_iterator<const T*>(cend()); }
  EIN(inline,artificial,const)
  constexpr std::reverse_iterator<const T*> crend() const noexcept { return std::reverse_iterator<const T*>(cbegin()); }

  EIN(inline,artificial,const)
  constexpr std::reverse_iterator<const T*> rbegin() const noexcept { return crbegin(); }
  EIN(inline,artificial,const)
  constexpr std::reverse_iterator<const T*> rend() const noexcept { return crend(); }

  // auto [x,y,z,w] = simd<float,4>(1.0,2.0,3.0,4.0);
  // auto & [x,y,z,w] = simd<float,4>(1.0,2.0,3.0,4.0);
  template <size_t I>
  requires (I < N)
  friend
  EIN(inline,artificial,const)
  constexpr T & get(simd & s) noexcept {
    return s[I];
  }

  template <size_t I>
  requires (I < N)
  friend
  EIN(inline,artificial,const)
  constexpr T const & get(simd const & s) noexcept {
    return s[I];
  }

  // auto && [x,y,z,w] = simd<float,4>(1.0,2.0,3.0,4.0);
  template <size_t I>
  requires (I < N)
  friend
  EIN(inline,artificial,const)
  constexpr T && get(simd && s) noexcept {
    return std::move(s[I]);  // Return rvalue reference
  }

  #define EIN_OP(op) \
    template <typename U> \
    requires (sizeof(U) == sizeof(T) && requires (simd_data_t<T,N> a, simd_data_t<U,N> b) { a op b; } ) \
    friend  \
    EIN(inline,artificial,pure) \
    constexpr simd_t<std::remove_cvref_t<decltype(std::declval<T>() op std::declval<U>())>> operator op(simd x, simd_t<U> y) noexcept { \
      return x.data op y.data; \
    } \
    template <typename U> \
    requires (sizeof(U) == sizeof(T) && requires (simd_data_t<T,N> a, simd_data_t<T,N> b) { a op##= b; }) \
    EIN(inline,artificial) \
    constexpr simd & operator op##=(simd_t<U> other) noexcept { \
      data op##= other.data; \
      return *this; \
    }

  EIN_OP(+)
  EIN_OP(-)
  EIN_OP(*)
  EIN_OP(/)
  EIN_OP(&)
  EIN_OP(|)
  EIN_OP(^)

  #undef EIN_OP

  #define EIN_UNARY_OP(op) \
    EIN(inline,artificial,pure) \
    constexpr simd_t<std::remove_cvref_t<decltype(op std::declval<T>())>> operator op() const noexcept \
    requires (requires (data_t x) { op x; }) { \
      return op data; \
    }

  EIN_UNARY_OP(+)
  EIN_UNARY_OP(-)
  EIN_UNARY_OP(~)
  EIN_UNARY_OP(!)

  #undef EIN_UNARY_OP

  EIN(inline,artificial)
  constexpr simd & operator--() noexcept
  requires (requires (data_t x) { --x; }) {
    --data;
    return *this;
  }

  EIN(inline,artificial)
  constexpr simd operator--(int) noexcept
  requires (requires (data_t x) { x--; }) {
    simd t = *this;
    data--;
    return t;
  }

  EIN(inline,artificial)
  constexpr simd & operator++() noexcept
  requires (requires (data_t x) { ++x; }) {
    ++data;
    return *this;
  }

  EIN(inline,artificial)
  constexpr simd operator++(int) noexcept
  requires (requires (data_t x) { x++; }) {
    simd t = *this;
    data++;
    return t;
  }

  template <size_t K>
  EIN(inline,artificial,pure)
  friend constexpr simd operator >>(simd x, imm_t<K>) noexcept
  requires requires (data_t a) { a >> static_cast<T>(K); } {
    return x.data >> static_cast<T>(K);
  }

  EIN(inline,artificial,pure)
  friend constexpr simd operator >>(simd x, T y) noexcept
  requires requires (data_t a, T y) { a >> y; } {
    return x.data >> y;
  }

  EIN(inline,artificial,pure)
  friend constexpr simd operator >>(simd x, simd y) noexcept
  requires requires (data_t a, data_t b) { a >> b; } {
    return x.data >> y.data;
  }

  // <<

  template <size_t K>
  EIN(inline,artificial,pure)
  friend constexpr simd operator <<(simd x, imm_t<K>) noexcept
  requires requires (data_t a) { a << static_cast<T>(K); } {
    return x.data << static_cast<T>(K);
  }

  EIN(inline,artificial,pure)
  friend constexpr simd operator <<(simd x, T y) noexcept
  requires requires (data_t x, T y) { x << y; } {
    return x.data << y;
  }

  EIN(inline,artificial,pure)
  friend constexpr simd operator <<(simd x, simd y) noexcept
  requires requires (data_t a, data_t b) { a << b; } {
    return x.data << y.data;
  }

  // >>=

  template <size_t K>
  EIN(inline,artificial)
  constexpr simd & operator >>=(imm_t<K>) noexcept
  requires requires (data_t a) { a >>= static_cast<T>(K); } {
    data >>= static_cast<T>(K);
    return *this;
  }

  EIN(inline,artificial)
  constexpr simd & operator >>=(simd y) noexcept
  requires (requires (data_t a) { a >>= a; }) {
    data >>= y.data;
    return *this;
  }

  template <size_t K>
  EIN(inline,artificial)
  constexpr simd & operator <<=(imm_t<K>) noexcept
  requires requires (data_t x) { x <<= K; } {
    data <<= K;
    return *this;
  }

  // <<=

  EIN(inline,artificial)
  constexpr simd & operator <<=(int y) noexcept
  requires requires (data_t x, int y) { x <<= y; } {
    data <<= y;
    return *this;
  }

  EIN(inline,artificial)
  constexpr simd & operator <<=(simd y) noexcept
  requires requires (data_t x) { x >>= x; } {
    data <<= y.data;
    return *this;
  }

  template <size_t ... is>
  EIN(inline,artificial)
  constexpr simd<T,sizeof...(is)> shuffle() noexcept
  requires (
    ((is < N) && ... && has_simd_type<T,sizeof...(is)>) &&
    requires (data_t x) { simd<T,sizeof...(is)>(__builtin_shufflevector(x, is...)); }
  ) {
    if consteval {
      return { data[is]... };
    } else {
      return __builtin_shufflevector(data,is...);
    }
  }

  template <size_t ... is>
  EIN(inline,artificial)
  constexpr simd<T,sizeof...(is)> shuffle(simd<T,N> b) noexcept
  requires (((is < N*2) && ... && has_simd_type<T,sizeof...(is)>) && requires (data_t x) { simd<T,sizeof...(is)>(__builtin_shufflevector(x, x, is...)); }) {
    if consteval {
      return { (is < N ? data[is] : b[is-N])... };
    } else {
      return __builtin_shufflevector(data,b.data,is...);
    }
  }

  // handle traditional comparisons

  EIN(inline,artificial,pure)
  friend constexpr mask_t operator < (simd x, simd y) noexcept
  requires (!has_mmask<T,N> && requires (data_t a) { { a < a } -> std::same_as<mask_t>; }) {
    return x.data < y.data;
  }

  EIN(inline,artificial,pure)
  friend constexpr mask_t operator > (simd x, simd y) noexcept
  requires (!has_mmask<T,N> && requires (data_t a) { { a > a } -> std::same_as<mask_t>; }) {
    return x.data > y.data;
  }

  EIN(inline,artificial,pure)
  friend constexpr mask_t operator <= (simd x, simd y) noexcept
  requires (!has_mmask<T,N> && requires (data_t a) { { a <= a } -> std::same_as<mask_t>; }) {
    return x.data <= y.data;
  }

  EIN(inline,artificial,pure)
  friend constexpr mask_t operator >= (simd x, simd y) noexcept
  requires (!has_mmask<T,N> && requires (data_t a) { { a >= a } -> std::same_as<mask_t>; }) {
    return x.data >= y.data;
  }

  EIN(inline,artificial,pure)
  friend constexpr mask_t operator == (simd x, simd y) noexcept
  requires (!has_mmask<T,N> && requires (data_t a) { { a == a } -> std::same_as<mask_t>; }) {
    return x.data == y.data;
  }

  EIN(inline,artificial,pure)
  friend constexpr mask_t operator != (simd x, simd y) noexcept
  requires (!has_mmask<T,N> && requires (data_t a) { { a != a } -> std::same_as<mask_t>; }) {
    return x.data != y.data;
  }

// avx512 comparisons

#ifdef __AVX512F__
  // implement masked compares

#define EIN_COMPARE_OP_SIZE(bs,cmd) \
         if constexpr (bytesize == bs) return cmd(a.it(),b.it()); \
    else
#define EIN_COMPARE_OP_TY(opfix,ty,infix) \
         if constexpr (std::is_same_v<T,ty>) { \
     EIN_COMPARE_OP_SIZE(16,_mm_cmp##opfix##_##infix##_mask) \
     EIN_COMPARE_OP_SIZE(32,_mm256_cmp##opfix##_##infix##_mask) \
     EIN_COMPARE_OP_SIZE(64,_mm512_cmp##opfix##_##infix##_mask) \
         static_assert(false); \
   } else

//  EIN(pure,artificial)
#define EIN_COMPARE_OP(op,opfix) \
  inline friend constexpr mask_t operator op (simd a, simd b) noexcept \
  requires has_mmask<T,N> && simd_builtin<T> { \
    if consteval { \
      mask_t result = 0; \
      for (size_t i=0;i<N;++i) \
        if (a[i] op b[i]) \
          result |= (1 << i); \
      return result; \
    } else { \
      EIN_COMPARE_OP_TY(opfix,float,ps) \
      EIN_COMPARE_OP_TY(opfix,double,pd) \
      EIN_COMPARE_OP_TY(opfix,int8_t,epi8) \
      EIN_COMPARE_OP_TY(opfix,int16_t,epi16) \
      EIN_COMPARE_OP_TY(opfix,int32_t,epi32) \
      EIN_COMPARE_OP_TY(opfix,int64_t,epi64) \
      EIN_COMPARE_OP_TY(opfix,uint8_t,epu8) \
      EIN_COMPARE_OP_TY(opfix,uint16_t,epu16) \
      EIN_COMPARE_OP_TY(opfix,uint32_t,epu32) \
      EIN_COMPARE_OP_TY(opfix,uint64_t,epu64) \
           static_assert(false); \
    } \
  }

  EIN_COMPARE_OP(<,lt)
  EIN_COMPARE_OP(>,gt)
  EIN_COMPARE_OP(>=,ge)
  EIN_COMPARE_OP(<=,le)
  EIN_COMPARE_OP(==,eq)
  EIN_COMPARE_OP(!=,neq)

#undef EIN_COMPARE_OP
#undef EIN_COMPARE_OP_TY
#undef EIN_COMPARE_OP_TY_SIZE
#endif

// CMP(FLOAT)

  template <CMP imm8>
  // EIN(nodiscard,inline,pure,artificial)
  EIN(nodiscard)
  friend constexpr mask_t cmp(simd a, simd b) noexcept
  requires one_of_t<T,float,double> && (imm8 < max_fp_comparison_predicate) {
    if consteval {
      // compile time polyfill
#ifdef __AVX512F__
      mask_t result = 0;
      for (size_t i=0;i<N;++i)
        if (cmp<imm8>(a[i],b[i]))
          result |= 1 << i;
      return result;
#define tron_suffix(x) x##_mask
#else
      mask_t result = 0;
      for (size_t i=0;i<N;++i)
        result[i] = cmp<imm8>(a[i],b[i]);
      return result;
#define tron_suffix(x) x
#endif

    } else {
      if constexpr(std::is_same_v<T,float>) {
             if constexpr (bytesize==16)  return tron_suffix(_mm_cmp_ps)(a.it(),b.it(),imm8);
        else if constexpr (bytesize==32)  return tron_suffix(_mm256_cmp_ps)(a.it(),b.it(),imm8);
#ifdef __AVX512F__
        else if constexpr (bytesize==64)  return tron_suffix(_mm512_cmp_ps)(a.it(),b.it(),imm8);
#endif
        else static_assert(false);
      } else if constexpr (std::is_same_v<T,double>) {
             if constexpr (bytesize==16)  return tron_suffix(_mm_cmp_pd)(a.it(),b.it(),imm8);
        else if constexpr (bytesize==32)  return tron_suffix(_mm256_cmp_pd)(a.it(),b.it(),imm8);
#ifdef __AVX512F__
        else if constexpr (bytesize==64)  return tron_suffix(_mm512_cmp_pd)(a.it(),b.it(),imm8);
#endif
#undef tron_suffix
        else static_assert(false);
      }
    }
  }

// CMPINT

  // EIN(nodiscard,inline,pure,artificial)
  template <CMPINT imm8>
  EIN(nodiscard,inline,pure,artificial) friend constexpr mask_t cmpint(simd a, simd b) noexcept
  requires one_of_t<T,uint8_t,int8_t,uint16_t,int16_t,uint32_t,int32_t,uint64_t,int64_t> && (size_t(imm8) < 8uz) {
    if consteval {
      // compile time polyfill
#ifdef __AVX512F__
      mask_t result = 0;
      for (size_t i=0;i<N;++i)
        if (cmpint<imm8>(a[i],b[i]))
          result |= 1 << i;
      return result;
#define tron_suffix _mask
#else
      mask_t result = 0;
      for (size_t i=0;i<N;++i)
        if (cmpint<imm8>(a[i],b[i]))
          result[i] = -1;
      return result;
#define tron_suffix
#endif
    } else {
#ifdef __AVX512F__
      #define EIN_HANDLE(type,infix) \
             if constexpr(std::is_same_v<T,type>) { \
             if constexpr (bytesize==16) return _mm_cmp_ ## infix ## _mask(a.it(),b.it(),imm8); \
        else if constexpr (bytesize==32) return _mm256_cmp_ ## infix ## _mask(a.it(),b.it(),imm8); \
        else if constexpr (bytesize==64) return _mm512_cmp_ ## infix ## _mask(a.it(),b.it(),imm8); \
        else static_assert(false); \
      } else
      EIN_HANDLE(int8_t,epi8)
      EIN_HANDLE(uint8_t,epu8)
      EIN_HANDLE(int16_t,epi16)
      EIN_HANDLE(uint16_t,epu16)
      EIN_HANDLE(int32_t,epi32)
      EIN_HANDLE(uint32_t,epu32)
      EIN_HANDLE(int64_t,epi64)
      EIN_HANDLE(uint64_t,epu64)
             static_assert(false);
      #undef EIN_HANDLE
#else
      // AVX-2 polyfill
             if constexpr (imm8 == CMPINT::FALSE) {
             if constexpr (bytesize==16) return _mm_setzero_si128();
        else if constexpr (bytesize==32) return _mm256_setzero_si256();
        else static_assert(false);
      } else if constexpr (imm8 == CMPINT::TRUE) {
             if constexpr (bytesize==16) return _mm_set1_epi32(-1);
        else if constexpr (bytesize==32) return _mm256_set1_epi32(-1);
        else static_assert(false);
      } else if constexpr (imm8 == CMPINT::EQ)  return a == b;
      else if constexpr (imm8 == CMPINT::NE)  return a != b;
      else if constexpr (imm8 == CMPINT::LT)  return a < b;
      else if constexpr (imm8 == CMPINT::NLE) return a > b;
      else if constexpr (imm8 == CMPINT::LE)  return a <= b;
      else if constexpr (imm8 == CMPINT::NLT) return a >= b;
      else static_assert(false);
#endif
    }
  }

#ifdef __AVX512F__
  // returning an mmask from comparisons
  // TODO: all other comparisons by macroing this
  #define EIN_COMPARE512_CASE(type,infix) \
           if constexpr(one_of_t<T,u##type,type>) { \
           if constexpr (bytesize==16) return _mm_cmpeq_##infix##_mask(a.it(),b.it()); \
      else if constexpr (bytesize==32) return _mm256_cmpeq_##infix##_mask(a.it(),b.it()); \
      else if constexpr (bytesize==64) return _mm512_cmpeq_##infix##_mask(a.it(),b.it()); \
      else static_assert(false); \
    } else
  #define EIN_COMPARE512(op,infix) \
  friend constexpr mask_t operator op (simd a, simd b) noexcept \
  requires has_mmask<T,N> && simd_builtin<T> { \
    if consteval { \
      mask_t mask; \
      for (size_t i=0;i<N;++i) \
       if (a[i] op b[i]) \
         mask |= 1 << i; \
      return mask; \
    } else { \
      if constexpr (one_of_t<T,float>) { \
             if constexpr (N==4)  return _mm_cmpeq_ps_mask(a.it(),b.it()); \
        else if constexpr (N==8)  return _mm256_cmpeq_ps_mask(a.it(),b.it()); \
        else if constexpr (N==16) return _mm512_cmpeq_ps_mask(a.it(),b.it()); \
        else static_assert(false); \
      } else if constexpr (one_of_t<T,double>) { \
             if constexpr (N==4)  return _mm_cmp##infix##_pd_mask(a.it(),b.it()); \
        else if constexpr (N==8)  return _mm256_cmp##infix##_pd_mask(a.it(),b.it()); \
        else if constexpr (N==16) return _mm512_cmp##infix##_pd_mask(a.it(),b.it()); \
        else static_assert(false); \
      } else \
      EIN_COMPARE512_CASE(int8_t,epi8) \
      EIN_COMPARE512_CASE(int16_t,epi16) \
      EIN_COMPARE512_CASE(int32_t,epi32) \
      EIN_COMPARE512_CASE(int64_t,epi64) \
      static_assert(false); \
    } \
  }

  EIN_COMPARE512(==,eq)
  EIN_COMPARE512(/=,neq)
  EIN_COMPARE512(<,lt)
  EIN_COMPARE512(<=,le)
  EIN_COMPARE512(>,gt)
  EIN_COMPARE512(>=,ge)

  #undef EIN_COMPARE512
#endif

  // loads and stores

#ifdef __AVX512F__
  #define EIN_SWITCH(on_m128,on_m128d,on_m128i,on_m256,on_m256d,on_m256i,on_m512,on_m512d,on_m512i) \
         if constexpr (std::is_same_v<intrinsic_t,__m128>) { EIN_CASE(on_m128) } \
    else if constexpr (std::is_same_v<intrinsic_t,__m128d>) { EIN_CASE(on_m128d) } \
    else if constexpr (std::is_same_v<intrinsic_t,__m128i>) { EIN_CASE(on_m128i) } \
    else if constexpr (std::is_same_v<intrinsic_t,__m256>) { EIN_CASE(on_m256) } \
    else if constexpr (std::is_same_v<intrinsic_t,__m256d>) { EIN_CASE(on_m256d) } \
    else if constexpr (std::is_same_v<intrinsic_t,__m256i>) { EIN_CASE(on_m256i) } \
    else if constexpr (std::is_same_v<intrinsic_t,__m512>) { EIN_CASE(on_m512) } \
    else if constexpr (std::is_same_v<intrinsic_t,__m512d>) { EIN_CASE(on_m512d) } \
    else if constexpr (std::is_same_v<intrinsic_t,__m512i>) { EIN_CASE(on_m512i) } \
    else static_assert(false);
#else
  #define EIN_SWITCH(on_m128,on_m128d,on_m128i,on_m256,on_m256d,on_m256i,on_m512,on_m512d,on_m512i) \
         if constexpr (std::is_same_v<intrinsic_t,__m128>) { EIN_CASE(on_m128) } \
    else if constexpr (std::is_same_v<intrinsic_t,__m128d>) { EIN_CASE(on_m128d) } \
    else if constexpr (std::is_same_v<intrinsic_t,__m128i>) { EIN_CASE(on_m128i) } \
    else if constexpr (std::is_same_v<intrinsic_t,__m256>) { EIN_CASE(on_m256) } \
    else if constexpr (std::is_same_v<intrinsic_t,__m256d>) { EIN_CASE(on_m256d) } \
    else if constexpr (std::is_same_v<intrinsic_t,__m256i>) { EIN_CASE(on_m256i) } \
    else static_assert(false);
#endif

#define EIN_CASE(f) return f(p);

  // loads

  // NB: could be a friend constexpr instead of static,
  // but then users would have to say T and not just N
  EIN(nodiscard,inline,pure,artificial)
  static constexpr simd load(T const * p) noexcept {
    if consteval {
      simd result;
      for (size_t i = 0;i<N;++i)
        result[i] = p[i];
      return result;
    } else {
      EIN_SWITCH(
        _mm_load_ps,    _mm_load_pd,    _mm_load_epi32,
        _mm256_load_ps, _mm256_load_pd, _mm256_load_epi32,
        _mm512_load_ps, _mm512_load_pd, _mm512_load_epi32
      )
    }
  }

  EIN(nodiscard,inline,pure,artificial)
  static constexpr simd loadu(T const * p) noexcept {
    if consteval {
      simd result;
      for (size_t i = 0;i<N;++i)
        result[i] = p[i];
      return result;
    } else {
      EIN_SWITCH(
        _mm_loadu_ps,    _mm_loadu_pd,    _mm_loadu_epi32,
        _mm256_loadu_ps, _mm256_loadu_pd, _mm256_loadu_epi32,
        _mm512_loadu_ps, _mm512_loadu_pd, _mm512_loadu_epi32
      )
    }
  }

  EIN(nodiscard,inline,pure,artificial)
  static constexpr simd stream_load(T const * p) noexcept {
    if consteval {
      simd result;
      for (size_t i = 0;i<N;++i)
        result[i] = p[i];
      return result;
    } else {
      #define tron_mm_steam_load_ps(x)    _mm_castsi128_ps   (_mm_stream_load_si128(x))
      #define tron_mm256_steam_load_ps(x) _mm256_castsi256_ps(_mm_stream_load_si256(x))
      #define tron_mm512_steam_load_ps(x) _mm512_castsi512_ps(_mm_stream_load_si512(x))
      #define tron_mm_steam_load_pd(x)    _mm_castsi128_pd   (_mm_stream_load_si128(x))
      #define tron_mm256_steam_load_pd(x) _mm256_castsi256_pd(_mm_stream_load_si256(x))
      #define tron_mm512_steam_load_pd(x) _mm512_castsi512_pd(_mm_stream_load_si512(x))
      EIN_SWITCH(
        tron_mm_stream_load_ps,    tron_mm_stream_load_pd,    _mm_stream_load_si128,
        tron_mm256_stream_load_ps, tron_mm256_stream_load_pd, _mm256_stream_load_si256,
        tron_mm512_stream_load_ps, tron_mm512_stream_load_pd, _mm512_stream_load_si512
      )
      #undef tron_mm_stream_load_ps
      #undef tron_mm256_stream_load_ps
      #undef tron_mm512_stream_load_ps
      #undef tron_mm_stream_load_pd
      #undef tron_mm256_stream_load_pd
      #undef tron_mm512_stream_load_pd
    }
  }

  // legacy: may outperform loadu when the data crosses a cache boundary
  EIN(nodiscard,inline,pure,artificial)
  static constexpr simd lddqu(T const * p) noexcept {
    if consteval {
      simd result;
      for (size_t i = 0;i<N;++i)
        result[i] = p[i];
      return result;
    } else {
      EIN_SWITCH(
        _mm_loadu_ps,    _mm_loadu_pd,    _mm_lddqu_si128,
        _mm256_loadu_ps, _mm256_loadu_pd, _mm256_lddqu_si128,
        _mm512_loadu_ps, _mm512_loadu_pd, _mm512_loadu_si128
      )
    }
  }
#undef EIN_CASE

  // stores

#define EIN_CASE(f) f(p,x.it());

  EIN(inline,artificial)
  friend constexpr void store(T * p, simd x) noexcept {
    if consteval {
      for (size_t i = 0;i<N;++i)
        p[i] = x.data[i];
    } else {
      EIN_SWITCH(
        _mm_store_ps,    _mm_store_pd,    _mm_store_epi32,
        _mm256_store_ps, _mm256_store_pd, _mm256_store_epi32,
        _mm512_store_ps, _mm512_store_pd, _mm512_store_epi32
      )
    }
  }

  EIN(inline,artificial)
  friend constexpr void storeu(T * p, simd x) noexcept {
    if consteval {
      for (size_t i = 0;i<N;++i)
        p[i] = x.data[i];
    } else {
      EIN_SWITCH(
        _mm_store_ps,    _mm_store_pd,    _mm_store_epi32,
        _mm256_store_ps, _mm256_store_pd, _mm256_store_epi32,
        _mm512_store_ps, _mm512_store_pd, _mm512_store_epi32
      )
    }
  }

  EIN(inline,artificial)
  friend constexpr void stream(T * p, simd x) noexcept {
    if consteval {
      for (size_t i = 0;i<N;++i)
        p[i] = x.data[i];
    } else {
      EIN_SWITCH(
        _mm_stream_ps,    _mm_stream_pd,    _mm_stream_si128,
        _mm256_stream_ps, _mm256_stream_pd, _mm256_stream_si256,
        _mm512_stream_ps, _mm512_stream_pd, _mm512_stream_si512
      )
    }
  }

#undef EIN_CASE
#undef EIN_SWITCH

  EIN(inline,artificial,pure)
  friend constexpr simd scalef(simd x, simd y) noexcept
  requires one_of_t<T,float,double> {
    if consteval {
      simd result;
      for (size_t i=0;i<N;++i)
        result[i] = scalef(x[i],y[i]);
      return result;
    } else {
      if constexpr (std::is_same_v<T,float>) {
             if constexpr (bytesize==16) return _mm_scalef_ps(x,y);
        else if constexpr (bytesize==32) return _mm256_scalef_ps(x,y);
#ifdef __AVX512F__
        else if constexpr (bytesize==64) return _mm512_scalef_ps(x,y);
#endif
        else static_assert(false);
      } else if constexpr (std::is_same_v<T,double>) {
             if constexpr (bytesize==16) return _mm_scalef_pd(x,y);
        else if constexpr (bytesize==32) return _mm256_scalef_pd(x,y);
#ifdef __AVX512F__
        else if constexpr (bytesize==64) return _mm512_scalef_pd(x,y);
#endif
        else static_assert(false);
      }
    }
  }

  EIN(inline,artificial)
  friend constexpr void swap(simd & x, simd & y) noexcept {
    if consteval {
      for (int i=0;i<N;++i)
        swap(x[i],y[i]);
    } else {
      swap(x.data,y.data);
    }
  }
};

// CTAD guides for intel -- these are fairly limited
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
export simd(__m128) -> simd<float,4>;
export simd(__m128d) -> simd<double,2>;
export simd(__m256) -> simd<float,8>;
export simd(__m256d) -> simd<double,4>;
#ifdef __AVX512F__
export simd(__m512) -> simd<float,16>;
export simd(__m512d) -> simd<double,8>;
#endif
#endif

// CTAD guidance for custom user types
export template <typename T, size_t N>
requires (
     has_simd_type<T,N>
  && (N % sizeof(T) == 0)
#if  defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
  && !one_of_t<T, long long, float, double>
#endif
)
simd(T __attribute ((__vector_size__(N)))) -> simd<T,N/sizeof(T)>;

export template <typename ... Args>
requires has_simd_type<std::common_type<Args...>, sizeof...(Args)>
simd(Args&&...) -> simd<std::common_type<Args...>,sizeof...(Args)>;

// default to max simd size for broadcast
export template <typename T>
requires has_simd_type<T,max_simd_size/sizeof(T)>
simd(T) -> simd<T,has_simd_type<T,max_simd_size/sizeof(T)>>;

// top level combinators for these have flipped arguments for slightly nicer inference
export template <std::size_t N, typename T>
EIN(nodiscard,inline,pure)
simd<T,N> load(T * data)
requires has_simd_type<T,N> {
  return simd<T,N>::load(data);
}

export template <std::size_t N, typename T>
EIN(nodiscard,inline,pure)
simd<T,N> loadu(T * data) noexcept
requires has_simd_type<T,N> {
  return simd<T,N>::loadu(data);
}

export template <std::size_t N, typename T>
EIN(nodiscard,inline,pure)
simd<T,N> stream_load(T * data) noexcept
requires has_simd_type<T,N> {
  return simd<T,N>::stream_load(data);
}

// compat

using fp32x4     = simd<float,4>;
using fp32x8     = simd<float,8>;
using fp64x2     = simd<double,2>;
using fp64x4     = simd<double,4>;
using int8x16    = simd<int8_t,16>;
using int8x32    = simd<int8_t,32>;
using int16x8    = simd<int16_t,8>;
using int16x16   = simd<int16_t,16>;
using int32x4    = simd<int32_t,4>;
using int32x8    = simd<int32_t,8>;
using int64x2    = simd<int64_t,2>;
using int64x4    = simd<int64_t,4>;
using uint8x16   = simd<uint8_t,16>;
using uint8x32   = simd<uint8_t,32>;
using uint16x8   = simd<uint16_t,8>;
using uint16x16  = simd<uint16_t,16>;
using uint32x4   = simd<uint32_t,4>;
using uint32x8   = simd<uint32_t,8>;
using uint64x2   = simd<uint64_t,2>;
using uint64x4   = simd<uint64_t,4>;

#ifdef __AVX512F__
using fp32x16    = simd<float,16>;
using fp64x8     = simd<double,8>;
using int8x64    = simd<int8_t,64>;
using int32x16   = simd<int32_t,16>;
using int16x32   = simd<int16_t,32>;
using int64x8    = simd<int64_t,8>;
using uint8x64   = simd<uint8_t,64>;
using uint16x32  = simd<uint16_t,32>;
using uint32x16  = simd<uint32_t,16>;
using uint64x8   = simd<uint64_t,8>;
#endif

#if 0
using bf16x8     = simd<bf16,8>;
using bf16x16    = simd<bf16,16>;

using fp16x8     = simd<fp16,8>;
using fp16x16    = simd<fp16,16>;

using fp8e5m2x16 = simd<fp8e5m2,16>;
using fp8e5m2x32 = simd<fp8e5m2,32>;

using fp8e4m3x16 = simd<fp8e4m3,16>;
using fp8e4m3x32 = simd<fp8e4m3,32>;

#ifdef __AVX512F__
using bf16x32    = simd<bf16,32>;
using fp16x32    = simd<fp16,32>;
using fp8e5m2x64 = simd<fp8e5m2,64>;
using fp8e4m3x64 = simd<fp8e4m3,64>;
#endif
#endif

} // namespace tron

namespace std {
  export template <typename T, size_t N>
  struct tuple_size<ein::simd<T, N>> : integral_constant<size_t, N> {};

  export template <size_t I, typename T, size_t N>
  requires (I < N)
  struct tuple_element<I, ein::simd<T, N>> {
    using type = T;
  };
}

