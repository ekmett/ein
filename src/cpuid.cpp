/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \ingroup cpuid */

#include <array>
#include <cstdint>
#include <string_view>
#include "attributes.hpp"
#include "cpuid.h"

namespace ein {

/// \hideinlinesource
ein_inline ein_pure cpuid_t cpuid(int32_t eax, int32_t ecx) noexcept {
  cpuid_t result;
  __cpuid_count(eax, ecx, result.eax, result.ebx, result.ecx, result.edx);
  return result;
}

/// \hideinitializer \hideinlinesource
const enum cpu_vendor cpu_vendor = [] static noexcept {
  auto result = cpuid(0, 0);
  std::array<int32_t, 3> data = { result.ebx, result.edx, result.ecx };
  std::string_view vendor {reinterpret_cast<char const *>(begin(data)), 12};
  using enum cpu_vendor;
  if      (vendor == "GenuineIntel") return intel;
  else if (vendor == "AuthenticAMD") return amd;
  else                               return unknown;
}();

} // end namespace ein
