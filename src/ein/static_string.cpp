/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

#include "static_string.hpp"

namespace ein {

extern template class basic_static_string<char,std::char_traits<char>>;
extern template class basic_static_string<wchar_t,std::char_traits<wchar_t>>;
extern template class basic_static_string<char8_t,std::char_traits<char8_t>>;
extern template class basic_static_string<char16_t,std::char_traits<char16_t>>;
extern template class basic_static_string<char32_t,std::char_traits<char32_t>>;

}

/// \cond
#if defined(EIN_TESTING) || defined(EIN_TESTING_STATIC_STRING)
#include <doctest.h>

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

template <typename... Types>
static constexpr void foreach_type(auto&& func) {
  (func.template operator()<Types>(), ...);  // Expands the lambda for each type
}

TEST_SUITE("static_string") {
  TEST_CASE("static_string constructors and conversions") {
    foreach_type<char,wchar_t,char8_t,char16_t,char32_t>([] <typename TestType> static constexpr {
      DOCTEST_SUBCASE(type<TestType>.data()) {
        using string_t = basic_static_string<TestType>;

        // Test construction from literal
        constexpr string_t str = STR(hello);
        CHECK(str == STR(hello));
        CHECK(str != STR(world));

        // Data retrieval and null termination
        CHECK(str.data()[5] == typename string_t::value_type(0));
      }
    });
  }

  TEST_CASE("static_string element access") {
    foreach_type<char,wchar_t,char8_t,char16_t,char32_t>([] <typename TestType> static constexpr {
      DOCTEST_SUBCASE(type<TestType>.data()) {
        using string_t = basic_static_string<TestType>;

        constexpr string_t str = STR(abcdef);
        CHECK(str.front() == 'a');
        CHECK(str.back() == 'f');
        CHECK(str[1] == 'b');
        CHECK(str.at(2) == 'c');

      }
    });

        //string_t str = STR(fedcba);
        // Test out-of-bounds access (should throw or be undefined for consteval in production)
        //CHECK_THROWS_AS(str.at(6), std::out_of_range);
  }

  TEST_CASE("static_string comparisons") {
    foreach_type<char,wchar_t,char8_t,char16_t,char32_t>([] <typename TestType> static constexpr {
      DOCTEST_SUBCASE(type<TestType>.data()) {
        using string_t = basic_static_string<TestType>;

        constexpr string_t str1 = STR(compare);
        constexpr string_t str2 = STR(compare);
        constexpr string_t str3 = STR(different);

        CHECK(str1 == str2);
        CHECK(str1 != str3);
      }
    });
  }

  TEST_CASE("static_string iterators") {
    foreach_type<char,wchar_t,char8_t,char16_t,char32_t>([] <typename TestType> static constexpr {
      DOCTEST_SUBCASE(type<TestType>.data()) {
        using string_t = basic_static_string<TestType>;

        constexpr string_t str = STR(iterate);
        std::basic_string<TestType> result;
        for (auto c : str)
          result += c;
        CHECK(result == STR(iterate).data());

        // Reverse iteration
        std::basic_string<typename string_t::value_type> reverse_result;
        for (auto it = str.rbegin(); it != str.rend(); ++it)
          reverse_result += *it;
        CHECK(reverse_result == STR(etareti).data());
      }
    });
  }

  TEST_CASE("basic_static_string I/O operations") {
    foreach_type<char,wchar_t>([] <typename TestType> static constexpr {
      DOCTEST_SUBCASE(type<TestType>.data()) {
        using string_t = basic_static_string<TestType>;
        constexpr string_t str = STR(output);
        std::basic_ostringstream<TestType> oss;
        oss << str;
        CHECK(oss.str()[3] == 'p');
      }
    });
  }

} // TEST_SUITE("static_string")

#endif // ENABLE_TESTS
/// \endcond
