// test_numerics.cpp

#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "ein/numerics.hpp"

using namespace ein;


TEST_CASE("Concepts: one_of and not_one_of","[numerics]") {
    REQUIRE(one_of<1, 1, 2, 3>);
    REQUIRE_FALSE(one_of<4, 1, 2, 3>);
    REQUIRE(not_one_of<4, 1, 2, 3>);
    REQUIRE_FALSE(not_one_of<1, 1, 2, 3>);
}

TEST_CASE("integer_traits","[numerics]") {
    REQUIRE(std::is_same_v<integer_traits<8>::signed_t, int8_t>);
    REQUIRE(std::is_same_v<integer_traits<16>::unsigned_t, uint16_t>);
    REQUIRE(std::is_same_v<integer_traits<32>::signed_t, int32_t>);
    REQUIRE(std::is_same_v<integer_traits<64>::unsigned_t, uint64_t>);
}

TEST_CASE("int_t and uint_t","[numerics]") {
    REQUIRE(std::is_same_v<int_t<int8_t>, int8_t>);
    REQUIRE(std::is_same_v<uint_t<int16_t>, uint16_t>);
    REQUIRE(std::is_same_v<int_t<int32_t>, int32_t>);
    REQUIRE(std::is_same_v<uint_t<int64_t>, uint64_t>);
}

TEST_CASE("imm_t compile-time constant","[numerics]") {
    constexpr auto imm4 = imm<4>;
    REQUIRE(imm4.value == 4);
    REQUIRE(static_cast<size_t>(imm4) == 4);
}

TEST_CASE("cmp_unord and cmp_ord","[numerics]") {
    REQUIRE(cmp_unord(NAN, 1.0f));
    REQUIRE(cmp_unord(1.0f, NAN));
    REQUIRE_FALSE(cmp_unord(1.0f, 2.0f));

    REQUIRE(cmp_ord(1.0f, 2.0f));
    REQUIRE_FALSE(cmp_ord(NAN, 1.0f));
    REQUIRE_FALSE(cmp_ord(1.0f, NAN));
}

TEST_CASE("scalef","[numerics]") {
    constexpr float x = 2.0f;
    constexpr float y = 3.0f;
    REQUIRE(scalef(x, y) == Catch::Approx(16.0f));

    constexpr double a = 4.0;
    constexpr double b = -2.0;
    REQUIRE(scalef(a, b) == Catch::Approx(1.0));
}

TEST_CASE("CMPINT comparison","[numerics]") {
    REQUIRE(cmpint<CMPINT::EQ>(5, 5));
    REQUIRE(cmpint<CMPINT::NE>(5, 4));
    REQUIRE_FALSE(cmpint<CMPINT::LT>(5, 4));
    REQUIRE(cmpint<CMPINT::LE>(5, 5));
    REQUIRE(cmpint<CMPINT::NLT>(5, 5));
}

#ifdef __AVX512F__
TEST_CASE("CMP floating-point comparison with AVX512","[numerics]") {
    REQUIRE(cmp<CMP::EQ_OQ>(1.0f, 1.0f));
    REQUIRE(cmp<CMP::LT_OS>(1.0f, 2.0f));
    REQUIRE_FALSE(cmp<CMP::GT_OS>(1.0f, 2.0f));
    REQUIRE(cmp<CMP::NEQ_UQ>(1.0f, NAN));
}
#endif
