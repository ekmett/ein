#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <string_view>
#include "ein/types.hpp"

using namespace ein;

struct Custom {};

TEST_CASE("type provides correct demangled names", "[type]") {
  SECTION("Basic types") {
    CHECK("int"s == type<int>);
    CHECK("double"s == type<double>);
  }

  SECTION("User-defined types") {
    CHECK(type<Custom> == "Custom"s);
  }

  SECTION("Pointers and References") {
    CHECK(type<int*> == "int*");
    CHECK(type<int&> == "int");
    CHECK(type<int&> == "int");
  }
}

TEST_CASE("type_of extracts type from object", "[type_of]") {
  SECTION("Literals and basic types") {
    int i = 42;
    double d = 3.14;
    CHECK(type_of(i) == "int");
    CHECK(type_of(d) == "double");
  }

  SECTION("Constant and reference types") {
    const int ci = 42;
    CHECK(type_of(ci) == "int");

    int i = 34;
    int& ri = i;
    REQUIRE(type_of(ri) == "int");
  }
}

TEST_CASE("one_of_t concept identifies types within list", "[concepts][one_of_t]") {
  SECTION("Valid and invalid cases") {
    STATIC_CHECK(one_of_t<int, int, double, float>);
    STATIC_REQUIRE_FALSE(one_of_t<char, int, double, float>);
  }
}

TEST_CASE("not_one_of_t concept identifies types outside of list", "[concepts][not_one_of_t]") {
  SECTION("Valid and invalid cases") {
    STATIC_CHECK(not_one_of_t<char, int, double, float>);
    STATIC_REQUIRE_FALSE(not_one_of_t<int, int, double, float>);
  }
}
