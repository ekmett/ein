/// \file
/// \ingroup cpuid
/// \license
/// SPDX-FileType: Source
/// SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
/// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
/// \endlicense
module;

#ifndef EIN_PCH
#include "prelude.hpp"
#endif
#include <cpuid.h>

using namespace std;

/// \ingroup cpuid
export module ein.cpuid;

namespace ein {
/// \defgroup cpuid cpuid
/// \{

/// result from calling the \CPUID instruction
export struct [[nodiscard]] cpuid_t {
  int32_t eax, ebx, ecx, edx;
};

/// \brief calls \CPUID and returns all of the registers that it responds with.
/// \details this can take a 100+ cycles and stalls everything waiting for writes
/// so please, do this outside of the critical path.
/// \hideinlinesource
export ein_inline ein_pure cpuid_t cpuid(int32_t eax, int32_t ecx) noexcept {
  cpuid_t result;
  __cpuid_count(eax, ecx, result.eax, result.ebx, result.ecx, result.edx);
  return result;
}

/// \brief CPU vendor id
export enum class [[nodiscard]] cpu_vendor {
  intel=0,  ///< \CPUID reported "GenuineIntel"
  amd=1,    ///< \CPUID reported "AuthenticAMD"
  unknown=2 ///< \CPUID reported something else
};

/// Reports the vendor of the CPU encountered at runtime. Cached.
/// \hideinitializer \hideinlinesource
export const enum cpu_vendor cpu_vendor = [] static noexcept {
  auto result = cpuid(0, 0);
  array<int32_t, 3> data = { result.ebx, result.edx, result.ecx };
  string_view vendor {reinterpret_cast<char const *>(begin(data)), 12};
  if      (vendor == "GenuineIntel") return cpu_vendor::intel;
  else if (vendor == "AuthenticAMD") return cpu_vendor::amd;
  else                               return cpu_vendor::unknown;
}();


/// \}
// end defgroup cpuid

} // end namespace ein
