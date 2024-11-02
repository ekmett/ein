/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

#include <doctest.h>
#include "cpuid.hpp"

using namespace ein;

TEST_SUITE("cpuid") {

  TEST_CASE("CPU vendor enum correctly identifies CPU") {
    CHECK((cpu_vendor == cpu_vendor::intel || cpu_vendor == cpu_vendor::amd));
  }

  TEST_CASE("CPUID function retrieves processor info and feature bits") {
    cpuid_t result = cpuid(1, 0);
    bool sse2_supported = result.edx & (1 << 26);
    CHECK(sse2_supported);
  }

} // TEST_SUITE("cpuid")
