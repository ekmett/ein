#include <catch2/catch_test_macros.hpp>
#include <array>
#include <cstring>
#include <string_view>
#include "ein/cpuid.hpp"

using namespace ein;


TEST_CASE("CPU vendor enum correctly identifies CPU", "[cpu_vendor]") {
    REQUIRE((cpu_vendor == cpu_vendor::intel || cpu_vendor == cpu_vendor::amd));
}

TEST_CASE("CPUID function retrieves processor info and feature bits", "[cpuid]") {
    cpuid_t result = cpuid(1, 0);
    bool sse2_supported = result.edx & (1 << 26);
    REQUIRE(sse2_supported);
}
