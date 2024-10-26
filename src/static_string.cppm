module;

#include "prelude.hpp"

using namespace std;

export module ein.static_string;

import ein.types;

namespace ein {

template <typename T, T...xs>
struct reify {
  static constexpr const T value[sizeof...(xs)+1] = {xs..., T() };
};

// forward declaration
export class static_c_string;

// statically known interned strings
// these have O(1) comparison for equality

// \invariant all such strings point to the start of the static member inside
//            operator""_ss for some string and their \p len is unmodified.
//            this ensures that they have O(1) comparison, because reference
//            and pointer equality coincide.

export template <
  typename CharT,
  typename Traits = std::char_traits<CharT>
>
class basic_static_string {
  basic_string_view<CharT,Traits> view;

  ein_inline constexpr
  basic_static_string(basic_string_view<CharT,Traits> view)
  : view(view) {}

  friend class static_c_string;
public:
  using traits_type = Traits;
  using value_type = CharT;
  using pointer = CharT *;
  using const_pointer = CharT const *;
  using reference = CharT &;
  using const_reference = CharT const &;
  using const_iterator = CharT const *;
  using iterator = const_iterator;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using reverse_iterator = const_reverse_iterator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  /// \name constructors
  /// \{

  template <char...xs>
  consteval explicit basic_static_string(std::integer_sequence<char,xs...>)
  : view(reify<CharT,xs...>::value,sizeof...(xs)) {}

  ein_inline constexpr
  basic_static_string() noexcept = default;

  ein_inline constexpr
  basic_static_string(basic_static_string const &) noexcept = default;

  ein_inline constexpr
  basic_static_string(basic_static_string const &&) noexcept = default;

  /// \}
  /// \name assignments
  /// \{

  ein_reinitializes ein_inline constexpr
  basic_static_string & operator = (basic_static_string const & that) noexcept = default;

  ein_reinitializes ein_inline constexpr
  basic_static_string & operator = (basic_static_string && that) noexcept = default;


  /// \}
  /// \name conversions
  /// \{

  ein_nodiscard ein_inline ein_pure constexpr
  operator CharT const * () const noexcept {
    return view.data; // an extra invariant that its null terminated is maintained
  }

  ein_nodiscard ein_inline ein_pure constexpr
  operator std::basic_string_view<CharT,Traits>() const noexcept {
    return view;
  }

  ein_nodiscard ein_inline ein_pure constexpr
  operator std::basic_string<CharT,Traits>() const noexcept {
    return view;
  }

  /// \}
  /// \name element access
  /// \{

  /// access the specified character
  ein_nodiscard ein_inline ein_pure constexpr
  CharT const & operator [](size_type i) const noexcept { return view[i]; }

  /// access the specified character with bounds checking
  ein_nodiscard ein_inline ein_pure constexpr
  CharT const & at(size_type i) const noexcept { return view.at(i); }

  /// \brief Returns reference to the first character in the static_string.
  ///
  /// \note
  ///   while the corresponding string_view operation is undefined if `empty()`,
  ///   this offers the slight weakening that it'll return `CharT(0)`.
  ///
  /// \pre !empty()
  ein_nodiscard ein_inline ein_pure constexpr
  const_reference front() const noexcept { return view.front(); }

  /// \pre !empty()
  ein_nodiscard ein_inline ein_pure constexpr
  const_reference back() const noexcept { return view.back(); }

  ein_nodiscard ein_inline ein_pure constexpr
  const_pointer data() const noexcept { return view.data(); }

  /// \}
  /// \name capacity
  /// \{

  ein_nodiscard ein_inline ein_pure constexpr
  size_type size() const noexcept { return view.size(); }

  ein_nodiscard ein_inline ein_pure constexpr
  size_type length() const noexcept { return view.length(); }

  ein_nodiscard ein_inline ein_pure constexpr
  size_type max_size() const noexcept { return view.max_size(); }

  ein_nodiscard ein_inline ein_pure constexpr
  bool empty() const noexcept { return view.empty(); }

  ein_nodiscard ein_inline ein_pure
  explicit operator bool () const noexcept { return view; }

  /// \}
  /// \name comparisons
  /// \{
  /// \pre `Traits::eq` is assumed reflexive even if overloaded
  ein_nodiscard ein_inline ein_pure ein_artificial friend constexpr
  bool operator == (basic_static_string self, basic_static_string that) noexcept {
    if constexpr (std::is_same_v<Traits,std::char_traits<CharT>> && one_of_t<CharT,char,wchar_t,char8_t,char16_t,char32_t>) {
      return self.view.data() == that.view.data();
    } else {
      return (self.view.data() == that.view.data()) || (self.view == that.view); // fallback required
    }
  }

  /// \pre `Traits::eq` is assumed reflexive
  ein_nodiscard ein_inline ein_pure ein_artificial friend constexpr
  bool operator != (basic_static_string self, basic_static_string that) noexcept {
    if constexpr (std::is_same_v<Traits,std::char_traits<CharT>> && one_of_t<CharT,char,wchar_t,char8_t,char16_t,char32_t>) {
      return self.view.data != that.view.data;
    } else {
      return (self.view.data != that.view.data) && (self.view != that.view);
    }
  }

  /// \}

  /// input/output
  /// \{

  /// may throw std::ios_base:failure if an exception is thrown during output.
  friend constexpr
  basic_ostream<CharT, Traits> & operator<<(basic_ostream<CharT, Traits>& os, basic_static_string<CharT, Traits> v) {
    return os << v.view;
  }


  /// \}
  /// \name iterators
  /// \{

  ein_nodiscard ein_inline ein_const ein_artificial constexpr
  auto begin() const noexcept { return view.begin(); }

  ein_nodiscard ein_inline ein_pure constexpr
  auto end() const noexcept { return view.end(); }

  ein_nodiscard ein_inline ein_const ein_artificial constexpr
  auto cbegin() const noexcept { return view.cbegin(); }

  ein_nodiscard ein_inline ein_pure constexpr
  auto cend() const noexcept { return view.cend(); }

  ein_nodiscard ein_inline ein_pure constexpr
  auto rbegin() const noexcept { return view.rbegin(); }

  ein_nodiscard ein_inline ein_pure constexpr
  auto rend() const noexcept { return view.rend(); }

  ein_nodiscard ein_inline ein_pure constexpr
  auto crbegin() const noexcept { return view.crbegin(); }

  ein_nodiscard ein_inline ein_pure constexpr
  auto crend() const noexcept { return view.crend(); }

  /// \}

  ein_inline constexpr friend
  void swap(basic_static_string & x, basic_static_string & y) noexcept {
    using std::swap;
    swap(x.view,y.view);
  }

  static constexpr size_type npos = basic_string_view<CharT,Traits>::npos;
};

//export template basic_static_string<char,std::char_traits<char>>;
//export template basic_static_string<wchar_t,std::char_traits<wchar_t>>;
//export template basic_static_string<char8_t,std::char_traits<char8_t>>;
//export template basic_static_string<char16_t,std::char_traits<char16_t>>;
//export template basic_static_string<char32_t,std::char_traits<char32_t>>;

using static_string = basic_static_string<char>;
using static_wstring = basic_static_string<wchar_t>;
using static_u8string = basic_static_string<char8_t>;
using static_u16string = basic_static_string<char16_t>;
using static_u32string = basic_static_string<char32_t>;

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wpedantic"

#  ifdef __clang__
#    pragma GCC diagnostic ignored "-Wgnu-string-literal-operator-template"
#  endif
#endif

// this is the only method by which these are initially constructed
// all other methods just trade them around

template <class T, T ...xs>
consteval static_string operator"" _ss() noexcept {
  return basic_static_string(std::integer_sequence<T,xs...>{});
}

#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif

export class static_c_string {
  const char * p;
public:

  using traits_type = std::char_traits<char>;
  using value_type = char;
  using pointer = char *;
  using const_pointer = char const *;
  using reference = char &;
  using const_reference = char const &;
  using const_iterator = char const *;
  using iterator = const_iterator;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using reverse_iterator = const_reverse_iterator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  /// \name constructors
  /// \{

  ein_inline constexpr
  static_c_string() noexcept = default;

  ein_inline constexpr
  static_c_string(static_c_string const &) noexcept = default;

  ein_inline constexpr
  static_c_string(static_c_string &&) noexcept = default;

  ein_inline constexpr
  static_c_string(static_string x) noexcept : p(x.data()) {}

  /// \}
  /// \name assignment
  /// \{

  ein_reinitializes ein_inline constexpr
  static_c_string & operator = (static_c_string const &) noexcept = default;

  ein_reinitializes ein_inline constexpr
  static_c_string & operator = (static_c_string &&) noexcept = default;

  /// \}
  /// \name conversion
  /// \{

  ein_nodiscard ein_inline ein_pure ein_artificial constexpr
  operator const char * () const noexcept { return p; }

  ein_nodiscard ein_inline ein_pure ein_artificial constexpr
  operator static_string () const noexcept { return string_view(p); }

  ein_nodiscard ein_inline ein_pure ein_artificial constexpr
  const char * data() const noexcept { return p; }

  /// \}
  /// \name comparison
  /// \{

  ein_nodiscard ein_inline ein_pure ein_artificial constexpr friend
  bool operator == (static_c_string x, static_c_string y) noexcept { return x.p == y.p; }

  ein_nodiscard ein_inline ein_pure ein_artificial constexpr friend
  bool operator != (static_c_string x, static_c_string y) noexcept { return x.p != y.p; }

  /// \}
  /// \name iterators
  /// \{

  /// O(1)
  ein_nodiscard ein_inline ein_const ein_artificial constexpr
  auto begin() const noexcept { return p; }

  ein_nodiscard ein_inline ein_pure constexpr
  auto end() const noexcept { return p + strlen(p); }

  /// O(1)
  ein_nodiscard ein_inline ein_const ein_artificial constexpr
  auto cbegin() const noexcept { return p; }

  /// O(1)
  ein_nodiscard ein_inline ein_pure constexpr
  auto cend() const noexcept { return p + strlen(p); }

  /// O(n)
  ein_nodiscard ein_inline ein_pure constexpr
  auto rbegin() const noexcept { return std::reverse_iterator<const char *>(end()); }

  /// O(1)
  ein_nodiscard ein_inline ein_pure constexpr
  auto rend() const noexcept { return std::reverse_iterator<const char *>(begin()); }

  /// O(n)
  ein_nodiscard ein_inline ein_pure constexpr
  auto crbegin() const noexcept { return std::reverse_iterator<const char *>(end()); }

  /// O(1)
  ein_nodiscard ein_inline ein_pure constexpr
  auto crend() const noexcept { return std::reverse_iterator<const char *>(begin()); }

  /// \}

  ein_inline friend
  void swap(static_c_string & x, static_c_string & y) noexcept {
    using std::swap;
    swap(x.p,y.p);
  }
};



#if 0
namespace {
([] consteval static {
constinit static_string x = "x"_ss;
static_assert(x == x);
constinit static_string y = "y"_ss;
static_assert(y == y);
static_assert(x != y);
constinit static_atring x2 = "x"_ss;
static_assert(x2 == x2);
static_assert(x == x2);
static_assert(y != x2);
})();
};
#endif


} // namespace ein

namespace std {
  export template <typename CharT, typename Traits>
  struct hash<::ein::basic_static_string<CharT,Traits>> {
    constexpr size_t operator()(::ein::basic_static_string<CharT,Traits> const & s) const noexcept {
      return reinterpret_cast<size_t>(s.data());
    }
  };
  export template <>
  struct hash<::ein::static_c_string> {
    constexpr size_t operator()(::ein::static_c_string s) const noexcept {
      return reinterpret_cast<size_t>(s.data());
    }
  };
  namespace ranges {

    /// \cond external specializations
    export template< class CharT, class Traits >
    constexpr bool enable_borrowed_range<::ein::basic_static_string<CharT, Traits>> = true;

    export template< class CharT, class Traits >
    constexpr bool enable_view<::ein::basic_static_string<CharT, Traits>> = true;

    export template <>
    constexpr bool enable_borrowed_range<::ein::static_c_string> = true;

    export template <>
    constexpr bool enable_view<::ein::static_c_string> = true;
    /// \endcond

  }
}  // namespace std
