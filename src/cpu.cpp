module;

using namespace std;

module ein.cpu;

namespace ein::cpu {

/// \hideinitializer \hideinlinesource
const enum vendor vendor = [] static noexcept -> enum vendor {
  auto result = id(0, 0);
  array<char, 12> vendor;
  *reinterpret_cast<int32_t*>(&vendor[0]) = result.ebx;
  *reinterpret_cast<int32_t*>(&vendor[4]) = result.edx;
  *reinterpret_cast<int32_t*>(&vendor[8]) = result.ecx;
  string_view vendorStr { begin(vendor), end(vendor) };
  if      (vendorStr == "GenuineIntel") return vendor::intel;
  else if (vendorStr == "AuthenticAMD") return vendor::amd;
  else                                  return vendor::unknown;
}();

} // ein::cpu
