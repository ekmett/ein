module;

#include <cpuid.h>


export module ein.cpu;

namespace ein::cpu {

/// \nodiscard
export struct [[nodiscard]] id_t {
  int32_t eax, ebx, ecx, edx;
};

/// \inline \pure
export /// \cond
EIN(inline,pure) /// \endcond
id_t id(int32_t eax, int32_t ecx) noexcept {
  id_t result;
  __cpuid_count(eax, ecx, result.eax, result.ebx, result.ecx, result.edx);
  return result;
}

/// \brief cpu vendor id
/// \nodiscard
export enum class [[nodiscard]] vendor {
  intel=0,
  amd=1,
  unknown=2
};

/// current cpu vendor
export extern const enum vendor vendor;

} // ein::cpu
