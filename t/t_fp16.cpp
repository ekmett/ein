#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "numerics/fp16.hpp"

using namespace ein;

TEST_CASE("default constructors and conversion","[fp16]") {
  REQUIRE(fp16(0.5f).content == Catch::Approx(0.5f));
  REQUIRE(0.25_fp16 .content == Catch::Approx(0.25f));
  REQUIRE(static_cast<float>(1.0_fp16) == Catch::Approx(1.0f));
  REQUIRE(static_cast<_Float16>(0.75_fp16) == Catch::Approx(_Float16(0.75f)));
}

TEST_CASE("comparison operators","[fp16]") {
  REQUIRE(0.5_fp16 == 0.5_fp16);
  REQUIRE(0.1_fp16 < 0.2_fp16);
  REQUIRE(0.3_fp16 > 0.2_fp16);
  REQUIRE(0.2_fp16 <= 0.2_fp16);
  REQUIRE(0.2_fp16 >= 0.2_fp16);
}

TEST_CASE("swap function","[fp16]") {
  fp16 x(1.0_fp16), y(2.0_fp16);
  swap(x, y);
  REQUIRE(x == fp16(2.0_fp16));
  REQUIRE(y == fp16(1.0_fp16));
}

TEST_CASE("bit operations","[fp16]") {
  fp16 val = fp16::from_bits(0x3C00);
  REQUIRE(val.to_bits() == 0x3C00);
  REQUIRE(fp16::from_bits(0x7C00) == std::numeric_limits<fp16>::infinity());
}

TEST_CASE("isnan function","[fp16]") {
  REQUIRE(std::isnan(fp16::from_bits(0x7FFF)));
  REQUIRE_FALSE(std::isnan(fp16(1.0_fp16)));
}

TEST_CASE("numeric_limits","[fp16]") {
  using nl = std::numeric_limits<fp16>;

  REQUIRE(nl::is_specialized);
  REQUIRE(nl::is_signed);
  REQUIRE(nl::digits == 11);
  REQUIRE(nl::epsilon() == fp16::from_bits(0x1200));
  REQUIRE(nl::round_error() == fp16::from_bits(0x3C00));
  REQUIRE(nl::min() == fp16::from_bits(0x0200));
  REQUIRE(nl::max() == fp16::from_bits(0x7BFF));
  REQUIRE(nl::lowest() == fp16::from_bits(0xFBFF));
  REQUIRE(nl::infinity() == fp16::from_bits(0x7C00));
  REQUIRE(std::isnan(nl::quiet_NaN()));
  REQUIRE(std::isnan(nl::signaling_NaN()));
  REQUIRE(nl::denorm_min() == fp16::from_bits(0));
}

