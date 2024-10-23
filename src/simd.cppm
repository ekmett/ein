module;

#include <immintrin.h>

using namespace std;

export module ein.simd;

import ein.numerics;
import ein.types;
import ein.simd_data;

namespace ein {

/// \brief simd primitive definition
export template <typename T, size_t N>
requires (has_simd_type<T,N>)
struct simd {
private:
  using data_t = simd_data_t<T,N>;
  using mask_t = simd_mask_t<T,N>;
  using intrinsic_t = simd_intrinsic_t<T,N>;
  static constexpr size_t bytesize = N*sizeof(T);

  template <typename U>
  using simd_t = simd<U,N>;

  template <typename U>
  using cast_t = simd<U,N*sizeof(T)/sizeof(U)>;

  using value_type = T;
  static constexpr size_t size = N;
public:

  data_t data;

  /// \brief default initialization
  /// \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd() noexcept = default;

  /// \brief copy construction
  /// \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd(simd const &) noexcept = default;

  /// \brief move construction
  /// \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd(simd &&) noexcept = default;

  /// \brief array initialization
  /// \details e.g. `simd<float,4>{1.0,2.0,3.0,4.0}`
  /// \inline \artificial \hidden
  template <std::convertible_to<T> ... Args>
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd(Args && ... args) noexcept
  requires (sizeof...(Args) == N)
  : data(std::forward<Args>(args)...) {}

  /// \brief broadcast construction
  /// \details sets every member of the simd structure to the same value
  /// \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd(T value) noexcept
  : data(__extension__(data_t)(value)) {}

  /// \brief copy construction from clang/gcc vector intrinsics
  /// \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd(data_t const & data) noexcept
  : data(data) {}

  /// \brief move construction from clang/gcc vector intrinsics
  /// \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd(data_t && data) noexcept
  : data(std::move(data)) {}

  // constexpr simd(const T (&list)[N]) :data(list) {}

  /// \brief initialize the first \p init `.size` values from an initializer_list
  /// TODO: use a masked unaligned load
  /// \inline \artificial \hidden
  /// \hideinlinesource
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd(std::initializer_list<T> init) {
    // NB: initializer_lists are janky af
    std::copy_n(init.begin(),std::min(N,init.size()),begin());
  }

  // TODO: append
  // concat
  //constexpr simd(simd<T,N/2> a, simd<T,N/2> b) : data(__builtin_shufflevector(a,b,...,...) {}

  /// \brief copy construct from the corresponding intel intrinsic type (if different than the gcc/clang one)
  /// \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd(intrinsic_t const & data) noexcept
  requires (!std::is_same_v<data_t, intrinsic_t>)
  : data(reinterpret_cast<data_t>(data)) {}

  /// \brief move construct from the corresponding intel intrinsic type (if different than the gcc/clang one)
  /// \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd(intrinsic_t && data) noexcept
  requires (!std::is_same_v<data_t, intrinsic_t>)
  : data(reinterpret_cast<data_t>(std::move(data))) {}

  /// \reinitializes \inline \artificial \hidden
  /** \cond */ EIN(reinitializes,inline,artificial,hidden) /** \endcond */
  constexpr simd & operator = (simd &&) noexcept = default;

  /// \reinitializes \inline \artificial \hidden
  /** \cond */ EIN(reinitializes,inline,artificial,hidden) /** \endcond */
  constexpr simd & operator = (simd const &) noexcept = default;

  /// automatic conversion for types supported by `__builtin_convertvector`
  /// \reinitializes \inline \artificial
  template <typename U>
  requires (!std::is_same_v<U,T> && has_builtin_convertvector<U,T,N>)
  /** \cond */ EIN(reinitializes,inline,artificial) /** \endcond */
  constexpr simd & operator = (simd_t<U> other) noexcept {
    if consteval {
      for (int i=0;i<N;++i)
        data[i] = T(other.data[i]);
    } else {
      data = __builtin_convertvector(other.data,data_t);
    }
  }

  /// provide compatibility with Intel intrinsics by freely using this as \ref simd_intrinsic_t<\p T,\p N> &
  /// \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr operator intrinsic_t & () noexcept {
    if constexpr (std::is_same_v<intrinsic_t,data_t>) return data;
    else return reinterpret_cast<intrinsic_t &>(data);
  }

  /// provide compatibility with Intel intrinsics by freely using this as \ref simd_intrinsic_t<\p T,\p N> const &
  /// \hideinlinesource \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr operator intrinsic_t const & () const noexcept {
    if constexpr (std::is_same_v<intrinsic_t,data_t>) return data;
    else return reinterpret_cast<intrinsic_t const &>(data);
  }

  /// provide compatibility with Intel intrinsics by freely using this as \ref simd_intrinsic_t<\p T,\p N> &
  /// \hideinlinesource \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr intrinsic_t & it() noexcept {
    if constexpr (std::is_same_v<intrinsic_t,data_t>) return data;
    else return reinterpret_cast<intrinsic_t &>(data);
  }

  /// provide compatibility with Intel intrinsics by freely using this as \ref simd_intrinsic_t<\p T,\p N> const &
  /// \hideinlinesource \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr intrinsic_t const & it() const noexcept {
    if constexpr (std::is_same_v<intrinsic_t,data_t>) return data;
    else return reinterpret_cast<intrinsic_t const &>(data);
  }

  /// access the element in the \p i th lane
  /// \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr T & operator[](std::size_t i) noexcept { return reinterpret_cast<T *>(&data)[i]; }

  /// access the element in the \p i th lane
  /// \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr T const & operator[](std::size_t i) const noexcept { return reinterpret_cast<T const *>(&data)[i]; }

  /// start iterator
  /// \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr T * begin() noexcept { return reinterpret_cast<T*>(&data); }

  /// end iterator
  /// \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr T * end() noexcept { return begin() + N; }

  /// const start iterator
  /// \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr T const * cbegin() const noexcept { return reinterpret_cast<T const *>(&data); }

  /// const end iterator
  /// \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr T const * cend() const noexcept { return begin() + N; }

  // const start iterator
  /// \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr T const * begin() const noexcept { return cbegin(); }

  // const end iterator
  /// \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr T const * end() const noexcept { return cend(); }

  /// reverse start iterator
  /// \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr std::reverse_iterator<T*> rbegin() noexcept { return std::reverse_iterator<T*>(end()); }

  /// reverse end iterator
  /// \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr std::reverse_iterator<T*> rend() noexcept { return std::reverse_iterator<T*>(begin()); }

  /// const reverse start iterator
  /// \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr std::reverse_iterator<const T*> crbegin() const noexcept { return std::reverse_iterator<const T*>(cend()); }

  /// const reverse end iterator
  /// \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr std::reverse_iterator<const T*> crend() const noexcept { return std::reverse_iterator<const T*>(cbegin()); }

  /// const reverse start iterator
  /// \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr std::reverse_iterator<const T*> rbegin() const noexcept { return crbegin(); }

  /// const reverse end iterator
  /// \inline \artificial \const \hidden
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr std::reverse_iterator<const T*> rend() const noexcept { return crend(); }

  /// \brief enables destructuring reference
  /// \details
  /// \code
  /// auto & [x,y,z,w] = simd<float,4>(1.0,2.0,3.0,4.0);
  /// \endcode
  /// \hideinlinesource \inline \artificial \const \hidden
  template <size_t I>
  requires (I < N)
  friend
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr T & get(simd & s) noexcept {
    return s[I];
  }

  /// \brief enables destructuring constant reference
  /// \details
  /// \code
  /// auto const & [x,y,z,w] = simd<float,4>(1.0,2.0,3.0,4.0);
  /// \endcode
  /// \hideinlinesource \inline \artificial \const \hidden
  template <size_t I>
  requires (I < N)
  friend
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr T const & get(simd const & s) noexcept {
    return s[I];
  }

  /// \brief enable destructuring move
  /// \details
  /// \code
  /// auto && [x,y,z,w] = simd<float,4>(1.0,2.0,3.0,4.0);
  /// \endcode
  /// \hideinlinesource \inline \artificial \const \hidden
  template <size_t I>
  requires (I < N)
  friend
  /** \cond */ EIN(inline,artificial,const,hidden) /** \endcond */
  constexpr T && get(simd && s) noexcept {
    return std::move(s[I]);  // Return rvalue reference
  }

/// \cond
  #define EIN_OP(op) \
    /** \hideinlinesource \inline \artificial \pure \hidden */ \
    template <typename U> \
    requires (sizeof(U) == sizeof(T) && requires (simd_data_t<T,N> a, simd_data_t<U,N> b) { a op b; } ) \
    friend  \
    /** \cond */ EIN(inline,artificial,pure,hidden) /** \endcond */ \
    constexpr simd_t<std::remove_cvref_t<decltype(std::declval<T>() op std::declval<U>())>> operator op(simd x, simd_t<U> y) noexcept { \
      return x.data op y.data; \
    } \
    /** \hideinlinesource \inline \artificial \hidden */ \
    template <typename U> \
    requires (sizeof(U) == sizeof(T) && requires (simd_data_t<T,N> a, simd_data_t<T,N> b) { a op##= b; }) \
    /** \cond */ EIN(inline,artificial,hidden) /** \endcond */ \
    constexpr simd & operator op##=(simd_t<U> other) noexcept { \
      data op##= other.data; \
      return *this; \
    }
/// \endcond

  EIN_OP(+)
  EIN_OP(-)
  EIN_OP(*)
  EIN_OP(/)
  EIN_OP(&)
  EIN_OP(|)
  EIN_OP(^)

  #undef EIN_OP

/// \cond
  #define EIN_UNARY_OP(op) \
    /** \hideinlinesource \inline \artificial \pure \hidden */ \
    /** \cond */ EIN(inline,artificial,pure,hidden) /** \endcond */ \
    constexpr simd_t<std::remove_cvref_t<decltype(op std::declval<T>())>> operator op() const noexcept \
    requires (requires (data_t x) { op x; }) { \
      return op data; \
    }
/// \endcond

  EIN_UNARY_OP(+)
  EIN_UNARY_OP(-)
  //EIN_UNARY_OP(~)
  //EIN_UNARY_OP(!)

  #undef EIN_UNARY_OP

  /// \brief `\--x`
  /// \hideinlinesource \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd & operator--() noexcept
  requires (requires (data_t x) { --x; }) {
    --data;
    return *this;
  }

  /// \brief `x--`
  /// \hideinlinesource \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd operator--(int) noexcept
  requires (requires (data_t x) { x--; }) {
    simd t = *this;
    data--;
    return t;
  }

  /// \brief `++x`
  /// \hideinlinesource \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd & operator++() noexcept
  requires (requires (data_t x) { ++x; }) {
    ++data;
    return *this;
  }

  /// \brief `x++`
  /// \hideinlinesource \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd operator++(int) noexcept
  requires (requires (data_t x) { x++; }) {
    simd t = *this;
    data++;
    return t;
  }

  /// \brief shift right by an immediate constant
  /// \hideinlinesource \inline \artificial \pure \hidden
  template <size_t K>
  requires requires (data_t a) { a >> static_cast<T>(K); }
  /** \cond */ EIN(inline,artificial,pure,hidden) /** \endcond */
  friend constexpr simd operator >>(simd x, imm_t<K>) noexcept {
    return x.data >> static_cast<T>(K);
  }

  /// \brief shift right by a scalar
  /// \hideinlinesource \inline \artificial \pure \hidden
  /** \cond */ EIN(inline,artificial,pure,hidden) /** \endcond */
  friend constexpr simd operator >>(simd x, T y) noexcept
  requires requires (data_t a, T y) { a >> y; } {
    return x.data >> y;
  }

  /// \brief shift right elementwise
  /// \hideinlinesource \inline \artificial \pure \hidden
  /** \cond */ EIN(inline,artificial,pure,hidden) /** \endcond */
  friend constexpr simd operator >>(simd x, simd y) noexcept
  requires requires (data_t a, data_t b) { a >> b; } {
    return x.data >> y.data;
  }

  // <<

  /// \brief shift left by an immediate constant
  /// \hideinlinesource \inline \artificial \pure \hidden
  template <size_t K>
  /** \cond */ EIN(inline,artificial,pure,hidden) /** \endcond */
  friend constexpr simd operator <<(simd x, imm_t<K>) noexcept
  requires requires (data_t a) { a << static_cast<T>(K); } {
    return x.data << static_cast<T>(K);
  }

  /// \brief shift left by a scalar
  /// \hideinlinesource \inline \artificial \pure \hidden
  /** \cond */ EIN(inline,artificial,pure,hidden) /** \endcond */
  friend constexpr simd operator <<(simd x, T y) noexcept
  requires requires (data_t x, T y) { x << y; } {
    return x.data << y;
  }

  /// \brief shift left elementwise
  /// \hideinlinesource \inline \artificial \pure \hidden
  /** \cond */ EIN(inline,artificial,pure,hidden) /** \endcond */
  friend constexpr simd operator <<(simd x, simd y) noexcept
  requires requires (data_t a, data_t b) { a << b; } {
    return x.data << y.data;
  }

  // >>=

  /// self-shift right by an immediate value
  /// \hideinlinesource \inline \artificial \hidden
  template <size_t K>
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd & operator >>=(imm_t<K>) noexcept
  requires requires (data_t a) { a >>= static_cast<T>(K); } {
    data >>= static_cast<T>(K);
    return *this;
  }

  /// self-shift right by a scalar
  /// \hideinlinesource \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd & operator >>=(T y) noexcept
  requires requires (data_t x, T y) { x >>= y; } {
    data >>= y;
    return *this;
  }

  /// self-shift right elementwise
  /// \hideinlinesource \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd & operator >>=(simd y) noexcept
  requires (requires (data_t a) { a >>= a; }) {
    data >>= y.data;
    return *this;
  }

  /// self-shift left by an immediate constant
  /// \hideinlinesource \inline \artificial \hidden
  template <size_t K>
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd & operator <<=(imm_t<K>) noexcept
  requires requires (data_t x) { x <<= K; } {
    data <<= K;
    return *this;
  }

  // <<=

  /// self-shift left by a scalar
  /// \hideinlinesource \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd & operator <<=(T y) noexcept
  requires requires (data_t x, T y) { x <<= y; } {
    data <<= y;
    return *this;
  }

  /// self-shift left elementwise
  /// \hideinlinesource \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
  constexpr simd & operator <<=(simd y) noexcept
  requires requires (data_t x) { x >>= x; } {
    data <<= y.data;
    return *this;
  }

  /// shuffle selected elements to produce a new simd register
  /// \inline \artificial
  template <size_t ... is>
  requires (((is < N) && ... && has_simd_type<T,sizeof...(is)>) &&
            requires (data_t x) { simd<T,sizeof...(is)>(__builtin_shufflevector(x, is...)); })
  /** \cond */ EIN(inline,artificial) /** \endcond */
  constexpr simd<T,sizeof...(is)> shuffle() noexcept {
    if consteval {
      return { data[is]... };
    } else {
      return __builtin_shufflevector(data,is...);
    }
  }

  /// Use elements taken from this and another simd register to construct another.
  /// If an index `i` in \p is is less than \p N, it draw from `this`, otherwise it draws from the `i - N`th position in \p b
  /// \inline \artificial \pure
  template <size_t ... is>
  requires (((is < N*2) && ... && has_simd_type<T,sizeof...(is)>) &&
            requires (data_t x) { simd<T,sizeof...(is)>(__builtin_shufflevector(x, x, is...)); })
  /** \cond */ EIN(inline,artificial,pure) /** \endcond */
  constexpr simd<T,sizeof...(is)> shuffle(simd<T,N> b) noexcept {
    if consteval {
      return { (is < N ? data[is] : b[is-N])... };
    } else {
      return __builtin_shufflevector(data,b.data,is...);
    }
  }

  // handle traditional comparisons

  /// \hideinlinesource \inline \artificial \pure \hidden
  /** \cond */ EIN(inline,artificial,pure,hidden) /** \endcond */
  friend constexpr mask_t operator < (simd x, simd y) noexcept
  requires (!has_mmask<T,N> && requires (data_t a) { { a < a } -> std::same_as<mask_t>; }) {
    return x.data < y.data;
  }

  /// \hideinlinesource \inline \artificial \pure \hidden
  /** \cond */ EIN(inline,artificial,pure,hidden) /** \endcond */
  friend constexpr mask_t operator > (simd x, simd y) noexcept
  requires (!has_mmask<T,N> && requires (data_t a) { { a > a } -> std::same_as<mask_t>; }) {
    return x.data > y.data;
  }

  /// \hideinlinesource \inline \artificial \pure \hidden
  /** \cond */ EIN(inline,artificial,pure,hidden) /** \endcond */
  friend constexpr mask_t operator <= (simd x, simd y) noexcept
  requires (!has_mmask<T,N> && requires (data_t a) { { a <= a } -> std::same_as<mask_t>; }) {
    return x.data <= y.data;
  }

  /// \hideinlinesource \inline \artificial \pure \hidden
  /** \cond */ EIN(inline,artificial,pure,hidden) /** \endcond */
  friend constexpr mask_t operator >= (simd x, simd y) noexcept
  requires (!has_mmask<T,N> && requires (data_t a) { { a >= a } -> std::same_as<mask_t>; }) {
    return x.data >= y.data;
  }

  /// \hideinlinesource \inline \artificial \pure \hidden
  /** \cond */ EIN(inline,artificial,pure,hidden) /** \endcond */
  friend constexpr mask_t operator == (simd x, simd y) noexcept
  requires (!has_mmask<T,N> && requires (data_t a) { { a == a } -> std::same_as<mask_t>; }) {
    return x.data == y.data;
  }

  /// \hideinlinesource \inline \artificial \pure \hidden
  /** \cond */ EIN(inline,artificial,pure,hidden) /** \endcond */
  friend constexpr mask_t operator != (simd x, simd y) noexcept
  requires (!has_mmask<T,N> && requires (data_t a) { { a != a } -> std::same_as<mask_t>; }) {
    return x.data != y.data;
  }

// avx512 comparisons

#ifdef __AVX512F__
  // implement masked compares

/** \cond */
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

#define EIN_COMPARE_OP(op,opfix) \
  /** \inline \artificial \pure */ \
  /** \cond */ EIN(inline,artificial,pure) /** \endcond */ \
  friend constexpr mask_t operator op (simd a, simd b) noexcept \
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
/** \endcond */

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

  /// \hideinlinesource \nodiscard \inline \artificial \pure
  template <CMP imm8>
  requires one_of_t<T,float,double> && (size_t(imm8) < max_fp_comparison_predicate)
  /** \cond */ EIN(nodiscard,inline,pure,artificial) /** \endcond */
  friend constexpr
  mask_t cmp(simd a, simd b) noexcept {
    if consteval {
      // compile time polyfill
#ifdef __AVX512F__
      mask_t result = 0;
      for (size_t i=0;i<N;++i)
        if (cmp<imm8>(a[i],b[i]))
          result |= 1 << i;
      return result;
/** \cond */
#define ein_suffix(x) x##_mask
/** \endcond */
#else
      mask_t result = 0;
      for (size_t i=0;i<N;++i)
        result[i] = cmp<imm8>(a[i],b[i]);
      return result;
/** \cond */
#define ein_suffix(x) x
/** \endcond */
#endif

    } else {
      if constexpr(std::is_same_v<T,float>) {
             if constexpr (bytesize==16)  return ein_suffix(_mm_cmp_ps)(a.it(),b.it(),imm8);
        else if constexpr (bytesize==32)  return ein_suffix(_mm256_cmp_ps)(a.it(),b.it(),imm8);
#ifdef __AVX512F__
        else if constexpr (bytesize==64)  return ein_suffix(_mm512_cmp_ps)(a.it(),b.it(),imm8);
#endif
        else static_assert(false);
      } else if constexpr (std::is_same_v<T,double>) {
             if constexpr (bytesize==16)  return ein_suffix(_mm_cmp_pd)(a.it(),b.it(),imm8);
        else if constexpr (bytesize==32)  return ein_suffix(_mm256_cmp_pd)(a.it(),b.it(),imm8);
#ifdef __AVX512F__
        else if constexpr (bytesize==64)  return ein_suffix(_mm512_cmp_pd)(a.it(),b.it(),imm8);
#endif
/** \cond */
#undef ein_suffix
/** \endcond */
        else static_assert(false);
      }
    }
  }

// CMPINT

  /// \hideinlinesource \nodiscard \inline \artificial \pure
  template <CMPINT imm8>
  requires one_of_t<T,uint8_t,int8_t,uint16_t,int16_t,uint32_t,int32_t,uint64_t,int64_t> && (size_t(imm8) < 8uz)
  /** \cond */ EIN(nodiscard,inline,artificial,pure) /** \endcond */
  friend constexpr
  mask_t cmpint(simd a, simd b) noexcept {
    if consteval {
      // compile time polyfill
#ifdef __AVX512F__
      mask_t result = 0;
      for (size_t i=0;i<N;++i)
        if (cmpint<imm8>(a[i],b[i]))
          result |= 1 << i;
      return result;
/** \cond */
#define ein_suffix _mask
/** \endcond */
#else
      mask_t result = 0;
      for (size_t i=0;i<N;++i)
        if (cmpint<imm8>(a[i],b[i]))
          result[i] = -1;
      return result;
/** \cond */
#define ein_suffix
/** \endcond */
#endif
    } else {
#ifdef __AVX512F__
      /// \cond
      #define EIN_HANDLE(type,infix) \
             if constexpr(std::is_same_v<T,type>) { \
             if constexpr (bytesize==16) return _mm_cmp_ ## infix ## _mask(a.it(),b.it(),imm8); \
        else if constexpr (bytesize==32) return _mm256_cmp_ ## infix ## _mask(a.it(),b.it(),imm8); \
        else if constexpr (bytesize==64) return _mm512_cmp_ ## infix ## _mask(a.it(),b.it(),imm8); \
        else static_assert(false); \
      } else
      /// \endcond
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
  /// \cond
  #define EIN_COMPARE512_CASE(type,infix) \
           if constexpr(one_of_t<T,u##type,type>) { \
           if constexpr (bytesize==16) return _mm_cmpeq_##infix##_mask(a.it(),b.it()); \
      else if constexpr (bytesize==32) return _mm256_cmpeq_##infix##_mask(a.it(),b.it()); \
      else if constexpr (bytesize==64) return _mm512_cmpeq_##infix##_mask(a.it(),b.it()); \
      else static_assert(false); \
    } else
  #define EIN_COMPARE512(op,infix) \
  /** \hideinlinesource \nodiscard \inline \pure */ \
  /** \cond */ EIN(nodiscard,inline,pure) /** \endcond */ \
  friend constexpr mask_t operator op (simd a, simd b) noexcept \
  requires (has_mmask<T,N> && simd_builtin<T>) { \
    if consteval { \
      mask_t mask; \
      for (size_t i=0;i<N;++i) \
       if (a[i] op b[i]) \
         mask |= 1 << i; \
      return mask; \
    } else { \
      if constexpr (one_of_t<T,float>) { \
             if constexpr (N==4)  return _mm_cmp##infix##_ps_mask(a.it(),b.it()); \
        else if constexpr (N==8)  return _mm256_cmp##infix##_ps_mask(a.it(),b.it()); \
        else if constexpr (N==16) return _mm512_cmp##infix##_ps_mask(a.it(),b.it()); \
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
  /// \endcond

  EIN_COMPARE512(==,eq)
  EIN_COMPARE512(/=,neq)
  EIN_COMPARE512(<,lt)
  EIN_COMPARE512(<=,le)
  EIN_COMPARE512(>,gt)
  EIN_COMPARE512(>=,ge)

  #undef EIN_COMPARE512
#endif

  // loads and stores

/// \cond
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

#define EIN_CASE(f) return f(reinterpret_cast<arg1_t<decltype(f)>>(p));
/// \endcond

  /// \pre \p p is a pointer to memory with alignment >= \p N
  /// \hideinlinesource \nodiscard \inline \pure \artificial
  /** \cond */ EIN(nodiscard,inline,pure,artificial) /** \endcond */
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

  /// \hideinlinesource \nodiscard \inline \pure \artificial
  /** \cond */ EIN(nodiscard,inline,pure,artificial) /** \endcond */
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

  /// \details legacy: may outperform loadu when the data crosses a cache boundary
  /// \hideinlinesource \nodiscard \inline \pure \artificial
  /** \cond */ EIN(nodiscard,inline,pure,artificial) /** \endcond */
  static constexpr simd lddqu(T const * p) noexcept {
    if consteval {
      simd result;
      for (size_t i = 0;i<N;++i)
        result[i] = p[i];
      return result;
    } else {
      EIN_SWITCH(
        _mm_loadu_ps,    _mm_loadu_pd,    _mm_lddqu_si128,
        _mm256_loadu_ps, _mm256_loadu_pd, _mm256_lddqu_si256,
        _mm512_loadu_ps, _mm512_loadu_pd, _mm512_loadu_si512
      )
    }
  }

/// \cond
#undef EIN_CASE
#define EIN_CASE(f) return f(p);
/// \endcond

  /// \pre \p p is a pointer to memory with alignment >= \p N
  /// \hideinlinesource \nodiscard \inline \pure \artificial
  /** \cond */ EIN(nodiscard,inline,pure,artificial) /** \endcond */
  static constexpr simd stream_load(T const * p) noexcept {
    if consteval {
      simd result;
      for (size_t i = 0;i<N;++i)
        result[i] = p[i];
      return result;
    } else {
/** \cond */
      #define ein_mm_stream_load_ps(x)    cast_ps(_mm_stream_load_si128(x))
      #define ein_mm256_stream_load_ps(x) cast_ps(_mm256_stream_load_si256(x))
      #define ein_mm512_stream_load_ps(x) cast_ps(_mm512_stream_load_si512(x))
      #define ein_mm_stream_load_pd(x)    cast_pd(_mm_stream_load_si128(x))
      #define ein_mm256_stream_load_pd(x) cast_pd(_mm256_stream_load_si256(x))
      #define ein_mm512_stream_load_pd(x) cast_pd(_mm512_stream_load_si512(x))
      EIN_SWITCH(
        ein_mm_stream_load_ps,    ein_mm_stream_load_pd,    _mm_stream_load_si128,
        ein_mm256_stream_load_ps, ein_mm256_stream_load_pd, _mm256_stream_load_si256,
        ein_mm512_stream_load_ps, ein_mm512_stream_load_pd, _mm512_stream_load_si512
      )

      #undef ein_mm_stream_load_ps
      #undef ein_mm256_stream_load_ps
      #undef ein_mm512_stream_load_ps
      #undef ein_mm_stream_load_pd
      #undef ein_mm256_stream_load_pd
      #undef ein_mm512_stream_load_pd
/** \endcond */
    }
  }

/// \cond
#undef EIN_CASE

#define EIN_CASE(f) f(p,x.it());
/// \endcond

  // stores
  // ======

  /// \hideinlinesource \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
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

  /// \hideinlinesource \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
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

  /// \hideinlinesource \inline \artificial \hidden
  /** \cond */ EIN(inline,artificial,hidden) /** \endcond */
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

  /// \hideinlinesource \nodiscard \inline \artificial \pure
  /** \cond */ EIN(nodiscard,inline,artificial,pure) /** \endcond */
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

  /// \hideinlinesource \inline \artificial
  /** \cond */ EIN(inline,artificial) /** \endcond */
  friend constexpr void swap(simd & x, simd & y) noexcept {
    if consteval {
      for (int i=0;i<N;++i)
        swap(x[i],y[i]);
    } else {
      swap(x.data,y.data);
    }
  }
};

/** \cond */
/// CTAD guides for intel -- these are fairly limited
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

/// CTAD guidance for custom user types
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

/// default to max simd size for broadcast
export template <typename T>
requires has_simd_type<T,max_simd_size/sizeof(T)>
simd(T) -> simd<T,has_simd_type<T,max_simd_size/sizeof(T)>>;
/** \endcond */

/// load \p data from aligned memory
/// \pre \p data has alignment >= \p N
/// \nodiscard \inline \pure \artificial
export
template <std::size_t N>
/** \cond */ EIN(nodiscard,inline,pure) /** \endcond */
auto load(auto const * data) noexcept -> simd<std::remove_cvref_t<decltype(*data)>,N> {
  using T = std::remove_cvref_t<decltype(*data)>;
  static_assert(has_simd_type<T,N>);
  return simd<T,N>::load(data);
}

/// load \p data from unaligned memory
/// \nodiscard \inline \pure \artificial
export template <std::size_t N>
/** \cond */ EIN(nodiscard,inline,pure) /** \endcond */
auto loadu(auto const * data) noexcept -> simd<std::remove_cvref_t<decltype(*data)>,N> {
  using T = std::remove_cvref_t<decltype(*data)>;
  static_assert(has_simd_type<T,N>);
  return simd<T,N>::loadu(data);
}

/// load \p data from unaligned memory, optimized for crossing cachelines (legacy approach)
/// \nodiscard \inline \pure \artificial
export template <std::size_t N>
/** \cond */ EIN(nodiscard,inline,pure,artificial) /** \endcond */
auto lddqu(auto const * data) noexcept -> simd<std::remove_cvref_t<decltype(*data)>,N> {
  using T = std::remove_cvref_t<decltype(*data)>;
  static_assert(has_simd_type<T,N>);
  return simd<T,N>::lddqu(data);
}

/// stream \p data from memory non-temporally, bypassing cache
/// \pre \p data has alignment >= \p N
/// \nodiscard \inline \pure \artificial
export template <std::size_t N>
/** \cond */ EIN(nodiscard,inline,pure,artificial) /** \endcond */
auto stream_load(auto const * data) noexcept -> simd<std::remove_cvref_t<decltype(*data)>,N> {
  using T = std::remove_cvref_t<decltype(*data)>;
  static_assert(has_simd_type<T,N>);
  return simd<T,N>::stream_load(data);
}

/// \cond
template <typename T>
struct simd_type_impl : std::false_type {};

template <typename T, size_t N>
requires has_simd_type<T,N>
struct simd_type_impl<simd<T,N>> : std::true_type {};
/// \endcond

/// recognizes any valid simd type
export template <typename SIMD>
concept simd_type = simd_type_impl<SIMD>::value;

/// create a new simd register with contents drawn from this one
/// \nodiscard \inline \artificial \pure
template <size_t ... is>
/** \cond */ EIN(nodiscard,inline,artificial,pure) /** \endcond */
auto shuffle(simd_type auto x) {
  return x.template shuffle<is...>();
}

/// create a new simd register with contents drawn from these two
/// \nodiscard \inline \artificial \pure
template <size_t ... is>
/** \cond */ EIN(nodiscard,inline,artificial,pure) /** \endcond */
auto shuffle(simd_type auto x, simd_type auto y) {
  return x.template shuffle<is...>(y);
}

//export template struct simd<float,4>;
export template struct simd<float,8>;
//export template struct simd<uint16_t,8>;
//export template struct simd<uint16_t,16>;
//export template struct simd<uint32_t,4>;
export template struct simd<uint32_t,8>;

} // namespace ein

namespace std {
  /// needed to support for std::apply
  export template <typename T, size_t N>
  struct tuple_size<ein::simd<T, N>> : integral_constant<size_t, N> {};

  /// needed to support for std::apply
  export template <size_t I, typename T, size_t N>
  requires (I < N)
  struct tuple_element<I, ein::simd<T, N>> {
    using type = T;
  };
}

