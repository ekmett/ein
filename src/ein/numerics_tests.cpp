/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \brief numerics external template instantiations */

#include <doctest.h>
#include "numerics.hpp"

using namespace ein;
using doctest::Approx;

TEST_SUITE("numerics") {

  TEST_CASE("Concepts: one_of and not_one_of") {
    CHECK(one_of<1, 1, 2, 3>);
    CHECK_FALSE(one_of<4, 1, 2, 3>);
    CHECK(not_one_of<4, 1, 2, 3>);
    CHECK_FALSE(not_one_of<1, 1, 2, 3>);
  }

  TEST_CASE("integer_traits") {
    CHECK(std::is_same_v<integer_traits<8>::signed_t, int8_t>);
    CHECK(std::is_same_v<integer_traits<16>::unsigned_t, uint16_t>);
    CHECK(std::is_same_v<integer_traits<32>::signed_t, int32_t>);
    CHECK(std::is_same_v<integer_traits<64>::unsigned_t, uint64_t>);
  }

  TEST_CASE("int_t and uint_t") {
    CHECK(std::is_same_v<int_t<int8_t>, int8_t>);
    CHECK(std::is_same_v<uint_t<int16_t>, uint16_t>);
    CHECK(std::is_same_v<int_t<int32_t>, int32_t>);
    CHECK(std::is_same_v<uint_t<int64_t>, uint64_t>);
  }

  TEST_CASE("imm_t compile-time constant") {
    constexpr auto imm4 = imm<4>;
    CHECK(imm4.value == 4);
    CHECK(static_cast<size_t>(imm4) == 4);
  }

  TEST_CASE("cmp_unord and cmp_ord") {
    CHECK(cmp_unord(NAN, 1.0f));
    CHECK(cmp_unord(1.0f, NAN));
    CHECK_FALSE(cmp_unord(1.0f, 2.0f));

    CHECK(cmp_ord(1.0f, 2.0f));
    CHECK_FALSE(cmp_ord(NAN, 1.0f));
    CHECK_FALSE(cmp_ord(1.0f, NAN));
  }

  TEST_CASE("scalef") {
    constexpr float x = 2.0f;
    constexpr float y = 3.0f;
    CHECK(scalef(x, y) == Approx(16.0f));

    constexpr double a = 4.0;
    constexpr double b = -2.0;
    CHECK(scalef(a, b) == Approx(1.0));
  }

  TEST_CASE("CMPINT comparison") {
    CHECK(cmpint<CMPINT::EQ>(5, 5));
    CHECK(cmpint<CMPINT::NE>(5, 4));
    CHECK_FALSE(cmpint<CMPINT::LT>(5, 4));
    CHECK(cmpint<CMPINT::LE>(5, 5));
    CHECK(cmpint<CMPINT::NLT>(5, 5));
  }

  #ifdef __AVX512F__
  TEST_CASE("CMP floating-point comparison with AVX512") {
    CHECK(cmp<CMP::EQ_OQ>(1.0f, 1.0f));
    CHECK(cmp<CMP::LT_OS>(1.0f, 2.0f));
    CHECK_FALSE(cmp<CMP::GT_OS>(1.0f, 2.0f));
    CHECK(cmp<CMP::NEQ_UQ>(1.0f, NAN));
  }
  #endif // __AVX512F__

} // TEST_SUITE("numerics")
