#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

#include <string_view>
#include <cstring>
#include <fstream>
#include <nlohmann/json.hpp>
#include "attributes/common.hpp"
#include "types.hpp"

namespace ein {

template <typename T, T...xs>
struct reify {
  static constexpr const T value[sizeof...(xs)+1] = {xs..., T() };
};

// forward declaration
class static_c_string;

/// statically known interned strings
/// these have O(1) comparison for equality
///
/// \invariant all such strings point to the start of the static member inside
///            operator""_ss for some string and their \p len is unmodified.
///            this ensures that they have O(1) comparison, because reference
///            and pointer equality coincide.

template <
  typename CharT,
  typename Traits = std::char_traits<CharT>
>
class basic_static_string {
  std::basic_string_view<CharT,Traits> view;

public:
  ein_inline constexpr
  basic_static_string(std::basic_string_view<CharT,Traits> view)
  : view(view) {}

  friend class static_c_string;
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

  template <CharT...xs>
  consteval explicit basic_static_string(std::integer_sequence<CharT,xs...>)
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
  CharT const & at(size_type i) const {
    if (i >= view.size()) throw std::out_of_range("basic_static_string::at");
    return view.at(i);
  }

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
      return self.view.data() != that.view.data();
    } else {
      return (self.view.data() != that.view.data()) && (self.view != that.view);
    }
  }

  /// \}
  /// input/output
  /// \{

  /// \throws std::ios_base:failure if an exception is thrown during output.
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

  static constexpr size_type npos = std::basic_string_view<CharT,Traits>::npos;
};

template <typename CharT, typename Traits>
void to_json(nlohmann::json& j, const basic_static_string<CharT,Traits>& s) {
  j = std::string(s);
}

/// \cond
template <typename CharT, CharT...xs>
basic_static_string(std::integer_sequence<CharT,xs...>) -> basic_static_string<CharT,std::char_traits<CharT>>;
extern template class basic_static_string<char,std::char_traits<char>>;
extern template class basic_static_string<wchar_t,std::char_traits<wchar_t>>;
extern template class basic_static_string<char8_t,std::char_traits<char8_t>>;
extern template class basic_static_string<char16_t,std::char_traits<char16_t>>;
extern template class basic_static_string<char32_t,std::char_traits<char32_t>>;
/// \endcond

using static_string = basic_static_string<char>;
using static_wstring = basic_static_string<wchar_t>;
using static_u8string = basic_static_string<char8_t>;
using static_u16string = basic_static_string<char16_t>;
using static_u32string = basic_static_string<char32_t>;

class static_c_string {
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
  // Standard C++23 string literal operator
};

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
consteval basic_static_string<T,std::char_traits<T>> operator"" _ss() noexcept {
  return basic_static_string<T,std::char_traits<T>>(std::integer_sequence<T,xs...>{});
}

template <one_of_t<char> T, T ...xs>
consteval static_c_string operator"" _scs() noexcept {
  return static_c_string(basic_static_string<T,std::char_traits<T>>(std::integer_sequence<T,xs...>{}));
}

#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif

inline void to_json(nlohmann::json& j, static_c_string const & s) {
  j = std::string(s);
}


} // namespace ein

namespace std {
  template <typename CharT, typename Traits>
  struct hash<::ein::basic_static_string<CharT,Traits>> {
    constexpr size_t operator()(::ein::basic_static_string<CharT,Traits> const & s) const noexcept {
      return reinterpret_cast<size_t>(s.data());
    }
  };
  template <>
  struct hash<::ein::static_c_string> {
    constexpr size_t operator()(::ein::static_c_string s) const noexcept {
      return reinterpret_cast<size_t>(s.data());
    }
  };
  namespace ranges {

    /// \cond external specializations
    template< class CharT, class Traits >
    inline constexpr bool enable_borrowed_range<::ein::basic_static_string<CharT, Traits>> = true;

    template< class CharT, class Traits >
    inline constexpr bool enable_view<::ein::basic_static_string<CharT, Traits>> = true;

    template <>
    inline constexpr bool enable_borrowed_range<::ein::static_c_string> = true;

    template <>
    inline constexpr bool enable_view<::ein::static_c_string> = true;
    /// \endcond
  }
}  // namespace std

#ifdef EIN_TESTING

#define EIN_STR(name) \
  (([]<typename CharT> static constexpr { \
    if constexpr (std::is_same_v<CharT, char>) { \
      return #name ## _ss; \
    } else if constexpr (std::is_same_v<CharT, wchar_t>) { \
      return L## #name ##_ss; \
    } else if constexpr (std::is_same_v<CharT, char8_t>) { \
      return u8## #name ##_ss; \
    } else if constexpr (std::is_same_v<CharT, char16_t>) { \
      return u## #name ##_ss; \
    } else if constexpr (std::is_same_v<CharT, char32_t>) { \
      return U## #name ##_ss; \
    } \
  }).template operator()<TestType>())

TEMPLATE_TEST_CASE("static_string","[static_string]",char,wchar_t,char8_t,char16_t,char32_t) {
  using namespace ein;
  using string_t = basic_static_string<TestType>;

  SECTION("constructors and conversions") {
    // Test construction from literal
    constexpr string_t str = EIN_STR(hello);
    CHECK(str == EIN_STR(hello));
    CHECK(str != EIN_STR(world));

    // Data retrieval and null termination
    CHECK(str.data()[5] == typename string_t::value_type(0));
  }

  SECTION("element access") {
    constexpr string_t str = EIN_STR(abcdef);
    CHECK(str.front() == 'a');
    CHECK(str.back() == 'f');
    CHECK(str[1] == 'b');
    CHECK(str.at(2) == 'c');
  }

  SECTION("comparisons") {
    constexpr string_t str1 = EIN_STR(compare);
    constexpr string_t str2 = EIN_STR(compare);
    constexpr string_t str3 = EIN_STR(different);

    CHECK(str1 == str2);
    CHECK(str1 != str3);
  }

  SECTION("iterators") {
    constexpr string_t str = EIN_STR(iterate);
    std::basic_string<TestType> result;
    for (auto c : str)
    result += c;
    CHECK(result == EIN_STR(iterate).data());

    // Reverse iteration
    std::basic_string<typename string_t::value_type> reverse_result;
    for (auto it = str.rbegin(); it != str.rend(); ++it)
      reverse_result += *it;
    CHECK(reverse_result == EIN_STR(etareti).data());
    }

  SECTION("I/O operations") {
    constexpr string_t str = EIN_STR(output);
    std::basic_ostringstream<TestType> oss;
    oss << str;
    CHECK(oss.str()[3] == 'p');
  }
}

#undef EIN_STR
#endif
