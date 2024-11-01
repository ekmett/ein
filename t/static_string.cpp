
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "ein/static_string.hpp" // Update to the correct path

using namespace ein;

#define STR(name) \
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

TEMPLATE_TEST_CASE("static_string constructors and conversions", "[static_string]", char, wchar_t, char8_t, char16_t, char32_t) {
  using string_t = basic_static_string<TestType>;

  // Test construction from literal
  constexpr string_t str = STR(hello);
  REQUIRE(str == STR(hello));
  REQUIRE(str != STR(world));

  // Data retrieval and null termination
  REQUIRE(str.data()[5] == typename string_t::value_type(0));
}

TEMPLATE_TEST_CASE("static_string element access", "[static_string]", char, wchar_t, char8_t, char16_t, char32_t) {
  using string_t = basic_static_string<TestType>;

  constexpr string_t str = STR(abcdef);
  REQUIRE(str.front() == 'a');
  REQUIRE(str.back() == 'f');
  REQUIRE(str[1] == 'b');
  REQUIRE(str.at(2) == 'c');


  //string_t str = STR(fedcba);
  // Test out-of-bounds access (should throw or be undefined for consteval in production)
  //REQUIRE_THROWS_AS(str.at(6), std::out_of_range);
}

TEMPLATE_TEST_CASE("static_string comparisons", "[static_string]", char, wchar_t, char8_t, char16_t, char32_t) {
  using string_t = basic_static_string<TestType>;

  constexpr string_t str1 = STR(compare);
  constexpr string_t str2 = STR(compare);
  constexpr string_t str3 = STR(different);

  REQUIRE(str1 == str2);
  REQUIRE(str1 != str3);
}

TEMPLATE_TEST_CASE("static_string iterators", "[static_string]",  char, wchar_t, char8_t, char16_t, char32_t) {
  using string_t = basic_static_string<TestType>;

  constexpr string_t str = STR(iterate);
  std::basic_string<TestType> result;
  for (auto c : str)
    result += c;
  REQUIRE(result == STR(iterate).data());

  // Reverse iteration
  std::basic_string<typename string_t::value_type> reverse_result;
  for (auto it = str.rbegin(); it != str.rend(); ++it)
    reverse_result += *it;
  REQUIRE(reverse_result == STR(etareti).data());
}

TEMPLATE_TEST_CASE("basic_static_string I/O operations", "[static_string]", char, wchar_t) {
  using string_t = basic_static_string<TestType>;
  constexpr string_t str = STR(output);
  std::basic_ostringstream<TestType> oss;
  oss << str;
  REQUIRE(oss.str()[3] == 'p');
}
