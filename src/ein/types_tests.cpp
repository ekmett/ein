
/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

#include <doctest.h>
#include <string_view>
#include "types.hpp"

using namespace ein;

namespace {
struct Custom {};
}

TEST_SUITE("types") {
  TEST_CASE("type provides correct demangled names") {
    DOCTEST_SUBCASE("Basic types") {
      CHECK("int"s == type<int>);
      CHECK("double"s == type<double>);
    }

    DOCTEST_SUBCASE("User-defined types") {
      CHECK(type<Custom> == "Custom"s);
    }

    DOCTEST_SUBCASE("Pointers and References") {
      CHECK(type<int*> == "int*");
      CHECK(type<int&> == "int");
      CHECK(type<int&> == "int");
    }
  }

  TEST_CASE("type_of extracts type from object") {
    DOCTEST_SUBCASE("Literals and basic types") {
      int i = 42;
      double d = 3.14;
      CHECK(type_of(i) == "int");
      CHECK(type_of(d) == "double");
    }

    DOCTEST_SUBCASE("Constant and reference types") {
      const int ci = 42;
      CHECK(type_of(ci) == "int");

      int i = 34;
      int& ri = i;
      REQUIRE(type_of(ri) == "int");
    }
  }

  TEST_CASE("one_of_t concept identifies types within list") {
    DOCTEST_SUBCASE("Valid and invalid cases") {
      REQUIRE(one_of_t<int, int, double, float>);
      REQUIRE_FALSE(one_of_t<char, int, double, float>);
    }
  }

  TEST_CASE("not_one_of_t concept identifies types outside of list") {
    DOCTEST_SUBCASE("Valid and invalid cases") {
      REQUIRE(not_one_of_t<char, int, double, float>);
      REQUIRE_FALSE(not_one_of_t<int, int, double, float>);
    }
  }
} // TEST_SUITE("types")
