#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

#include <cstdint>
#include "attributes/common.hpp"

using namespace std;

namespace ein {

/// result from calling the \CPUID instruction
struct ein_nodiscard cpuid_t {
  int32_t eax, ebx, ecx, edx;
};

/** \brief calls \CPUID and returns all of the registers that it responds with.

    \details
      this can take a 100+ cycles and stalls everything waiting for writes,
      so please, do this outside of the critical path.

    \hideinlinesource */

ein_pure extern cpuid_t cpuid(int32_t eax, int32_t ecx) noexcept;

/// \brief CPU vendor id
enum class ein_nodiscard cpu_vendor : uint8_t {
  intel=0,  ///< \CPUID reported \c "GenuineIntel"
  amd=1,    ///< \CPUID reported \c "AuthenticAMD"
  unknown=2 ///< \CPUID reported something else
};

/// Reports the vendor of the CPU encountered at runtime. Cached.
extern const enum cpu_vendor cpu_vendor;

} // end namespace ein

#if defined(EIN_TESTING) || defined(EIN_TESTING_CPUID)
TEST_CASE("cpuid","[cpuid]") {
  using namespace ein;

  SECTION("CPU vendor enum correctly identifies CPU") {
    CHECK((cpu_vendor == cpu_vendor::intel || cpu_vendor == cpu_vendor::amd));
  }

  SECTION("CPUID function retrieves processor info and feature bits") {
    cpuid_t result = cpuid(1, 0);
    bool sse2_supported = result.edx & (1 << 26);
    CHECK(sse2_supported);
  }
}
#endif
