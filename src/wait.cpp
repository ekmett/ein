#include "wait.hpp"

/// \hideinitializer \hideinlinesource
const bool mwaitx::supported = [] static noexcept {
  return (cpu_vendor == cpu_vendor::amd)
      && ((cpuid(0x80000001,0).ecx & (1 << 29)) != 0);
}();

/// \hideinitializer \hideinlinesource
const bool umwait::supported = [] static noexcept {
  return (cpu_vendor == cpu_vendor::intel)
      && ((cpuid(0x7,0).ecx & (1 << 5)) != 0);
}();
