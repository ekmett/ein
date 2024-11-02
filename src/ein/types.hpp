#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \ingroup types */

#include <string>
#include <string_view>
#include <cxxabi.h>
#include "attributes/common.hpp"

using namespace std;

/// \ingroup types

namespace ein {
/// \defgroup types Types
/// \{

/// \brief returns the unmangled name of a the type \p T
/// \details Ideally this would be fully `constexpr`, but I'm not building my own demangler.
/// \hideinitializer \hideinlinesource
template <typename T>
const string_view type = [] ein_nodiscard ein_const static noexcept -> string_view {
  // nb: copying into string is necessary because the demangled name is ephemeral
  static const string body = [] ein_nodiscard ein_const static noexcept -> char const * {
    int status;
    return abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
  }();
  return body;
}();

/// \brief returns the unmangled name of a the type of the (unused) argument passed to this function
/// \hideinlinesource
ein_nodiscard ein_const
const string_view type_of(auto const & t) noexcept {
  return type<remove_cvref_t<decltype(t)>>;
}

/// \brief type \p T is one of the \p candidates
template <typename T, typename ... candidates>
concept one_of_t = (std::is_same_v<T,candidates> || ... || false);

/// \brief type \p T is not one of the \p candidates
template <typename T, typename ... candidates>
concept not_one_of_t = (!one_of_t<T,candidates...>);
/// \}
}

#if defined(EIN_TESTING) || defined(EIN_TESTING_TYPES)
#include <string_view>
#include "types.hpp"

struct Custom {};

TEST_CASE("type provides correct demangled names","[types]") {
  using namespace ein;
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

TEST_CASE("type_of extracts type from object","[types]") {
  using namespace ein;
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

TEST_CASE("one_of_t concept identifies types within list","[types]") {
  using namespace ein;
  SECTION("Valid and invalid cases") {
    REQUIRE(one_of_t<int, int, double, float>);
    REQUIRE_FALSE(one_of_t<char, int, double, float>);
  }
}

TEST_CASE("not_one_of_t concept identifies types outside of list","[types]") {
  using namespace ein;
  SECTION("Valid and invalid cases") {
    REQUIRE(not_one_of_t<char, int, double, float>);
    REQUIRE_FALSE(not_one_of_t<int, int, double, float>);
  }
}
#endif
