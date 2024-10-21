module;

#if defined(_MSC_VER)
  #include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
  #include <cpuid.h>
#endif

/// \internal
static void cpuid(int32_t info[4], int32_t eax, int32_t ecx) noexcept {
#if defined(_MSC_VER)
  __cpuidex(info, eax, ecx);
#elif defined(__GNUC__) || defined(__clang__)
  __cpuid_count(eax, ecx, info[0], info[1], info[2], info[3]);
#endif
}

using namespace std;

module ein.cpu;

namespace ein::cpu {

const enum vendor vendor = []() noexcept -> enum vendor {
  int32_t info[4] = {0};
  cpuid(info, 0, 0);

  array<char, 12> vendor;
  memcpy(&vendor[0], &info[1], 4); // EBX
  memcpy(&vendor[4], &info[3], 4); // EDX
  memcpy(&vendor[8], &info[2], 4); // ECX

  string_view vendorStr { begin(vendor), end(vendor) };

  if      (vendorStr == "GenuineIntel") return vendor::intel;
  else if (vendorStr == "AuthenticAMD") return vendor::amd;
  else                                  return vendor::unknown;
}();

const bool has_mwait = []() noexcept -> bool {
  if (vendor != vendor::intel) return false;
  int32_t info[4] = {0};
  // CPUID.05H to check Intel MONITOR/MWAIT
  cpuid(info, 0x05, 0x00);
  // ECX[3] indicates if MONITOR/MWAIT can be executed in user mode on Intel
  return (info[2] & (1 << 3)) != 0;
}();

const bool has_mwaitx = []() noexcept -> bool {
  if (vendor != vendor::amd) return false;
  int32_t info[4] = {0};
  // CPUID.80000001H to check AMD MONITORX/MWAITX
  cpuid(info, 0x80000001, 0x00);
  // ECX[29] indicates MONITORX/MWAITX availability on AMD
  return (info[2] & (1 << 29)) != 0;
}();

} // ein::cpu
