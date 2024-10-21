module;

import <cstdint>;

export module ein.mwait;

namespace ein {

// Intel
// -----

EIN(export,inline)
void monitor(void * addr) {
  // MONITOR: rAX = addr, ECX = extensions, EDX = hints
  __asm__ __volatile__ (
    "monitor"
    : // No output
    : "rax"(addr), "rcx"(0), "rdx"(0) // Input: addr, ECX = 0, EDX = 0
    : "memory" // Clobbered registers (memory is implied by monitor)
  );
}

// intel provides mwait. while it isn't _documented_ to support the timer options from amd
// on newer cpus it does.
EIN(export,inline)
void mwait(uint32_t hint = 0, uint32_t extensions = 0, uint32_t timer = 0) {
  __asm__ volatile("mwait" : : "a"(hint), "c"(extensions), "d"(timer));
}

// AMD
// ---

EIN(export,inline)
void monitorx(void * addr) {
  // MONITORX: rAX = addr, ECX = extensions, EDX = hints
  __asm__ __volatile__ (
    "monitorx"
    : // No output
    : "rax"(addr), "rcx"(0), "rdx"(0) // Input: addr, ECX = 0, EDX = 0
    : "memory" // Clobbered registers (memory is implied by monitorx)
  );
}

// amd provides mwaitx for userland mwait instead of mwait.
EIN(export,inline)
void mwaitx(uint32_t hint = 0, uint32_t extensions = 0, uint32_t timer = 0) {
  __asm__ volatile("mwaitx" : : "a"(hint), "c"(extensions), "d"(timer));
}

} // namespace ein
