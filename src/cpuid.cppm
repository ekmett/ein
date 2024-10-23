/// \file
/// \ingroup cpuid
/// \license
/// SPDX-FileType: Source
/// SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
/// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
/// \endlicense
module;

#include <cpuid.h>

using namespace std;

/// \ingroup cpuid
export module ein.cpuid;

namespace ein {
/// \defgroup cpuid cpuid
/// \{

/// \nodiscard
export struct [[nodiscard]] cpuid_t { int32_t eax, ebx, ecx, edx; };

/// \hideinlinesource \inline \pure
export /// \cond
EIN(inline,pure) /// \endcond
cpuid_t cpuid(int32_t eax, int32_t ecx) noexcept {
  cpuid_t result;
  __cpuid_count(eax, ecx, result.eax, result.ebx, result.ecx, result.edx);
  return result;
}

/// \brief cpu vendor id
/// \nodiscard
export enum class [[nodiscard]] cpu_vendor {
  intel=0,
  amd=1,
  unknown=2
};

/// current cpu vendor
/// \hideinitializer \hideinlinesource
export const enum cpu_vendor cpu_vendor = [] static noexcept {
  auto result = cpuid(0, 0);
  array<char, 12> vendor;
  *reinterpret_cast<int32_t*>(&vendor[0]) = result.ebx;
  *reinterpret_cast<int32_t*>(&vendor[4]) = result.edx;
  *reinterpret_cast<int32_t*>(&vendor[8]) = result.ecx;
  string_view vendorStr { begin(vendor), end(vendor) };
  if      (vendorStr == "GenuineIntel") return cpu_vendor::intel;
  else if (vendorStr == "AuthenticAMD") return cpu_vendor::amd;
  else                                  return cpu_vendor::unknown;
}();


/// \}
} // namespace ein
