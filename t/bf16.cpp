#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "numerics/bf16.hpp"

using namespace ein;

TEST_CASE("bf16 default constructors and conversion", "[bf16]") {
  REQUIRE(bf16(0.5f).content == Catch::Approx(0.5f));
  REQUIRE(0.25_bf16 .content == Catch::Approx(0.25f));
  REQUIRE(static_cast<float>(1.0_bf16) == Catch::Approx(1.0f));
}

TEST_CASE("bf16 comparison operators", "[bf16]") {
  REQUIRE(0.5_bf16 == 0.5_bf16);
  REQUIRE(0.1_bf16 < 0.2_bf16);
  REQUIRE(0.3_bf16 > 0.2_bf16);
  REQUIRE(0.2_bf16 <= 0.2_bf16);
  REQUIRE(0.2_bf16 >= 0.2_bf16);

  // Negative checks for inequality
  REQUIRE_FALSE(0.5_bf16 < 0.5_bf16);
  REQUIRE_FALSE(0.3_bf16 < 0.2_bf16);
  REQUIRE_FALSE(0.1_bf16 > 0.2_bf16);
  REQUIRE_FALSE(0.3_bf16 <= 0.2_bf16);
  REQUIRE_FALSE(0.1_bf16 >= 0.2_bf16);
}

TEST_CASE("bf16 swap function", "[bf16]") {
  bf16 x(1.0_bf16), y(2.0_bf16);
  swap(x, y);
  REQUIRE(x == 2.0_bf16);
  REQUIRE(y == 1.0_bf16);
}

TEST_CASE("bf16 bit operations", "[bf16]") {
  bf16 val = bf16::from_bits(0x3F00);
  REQUIRE(val.to_bits() == 0x3F00);
  REQUIRE(bf16::from_bits(0x7F80) == std::numeric_limits<bf16>::infinity());
}

TEST_CASE("bf16 isnan function", "[bf16]") {
  REQUIRE(std::isnan(bf16::from_bits(0x7FC0)));
  REQUIRE_FALSE(std::isnan(1.0_bf16));
}

TEST_CASE("bf16 numeric_limits", "[bf16]") {
  using nl = std::numeric_limits<bf16>;

  REQUIRE(nl::is_specialized);
  REQUIRE(nl::is_signed);
  REQUIRE(nl::digits == 7);
  REQUIRE(nl::epsilon() == bf16::from_bits(0x3C00));
  REQUIRE(nl::round_error() == bf16::from_bits(0x3F00));
  REQUIRE(nl::min() == bf16::from_bits(0x007F));
  REQUIRE(nl::max() == bf16::from_bits(0x7F7F));
  REQUIRE(nl::lowest() == bf16::from_bits(0xFF7F));
  REQUIRE(nl::infinity() == bf16::from_bits(0x7F80));
  REQUIRE(std::isnan(nl::quiet_NaN()));
  REQUIRE(std::isnan(nl::signaling_NaN()));
  REQUIRE(nl::denorm_min() == bf16::from_bits(0));
}

