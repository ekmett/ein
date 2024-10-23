module;

import ein.cpu;

module ein.wait;

namespace ein::wait {

/// \hideinitializer \hideinlinesource
const bool mwaitx::supported = [] static noexcept -> bool {
  return (cpu::vendor == cpu::vendor::amd) && ((cpu::id(0x80000001,0).ecx & (1 << 29)) != 0);
}();

/// \hideinitializer \hideinlinesource
const bool umwait::supported = [] static noexcept -> bool {
  return (cpu::vendor == cpu::vendor::intel) && ((cpu::id(0x7,0).ecx & (1 << 5)) != 0);
}();

} // ein::wait
