module;

import <cstdint>;

export module mwait;

#if defined(__INTEL_COMPILER) || defined(__GNUC__) || defined(_MSC_VER) && defined(__x86_64__)
export EIN(inline)
void mwait(uint32_t hint = 0, uint32_t extensions = 0) {
    __asm__ volatile("mwait" : : "a"(hint), "c"(extensions));
}
#endif

#if defined(__amd64__) || defined(__x86_64__)
export EIN(inline)
void mwaitx(uint32_t hint = 0, uint32_t extensions = 0, uint32_t timer = 0) {
    __asm__ volatile("mwaitx" : : "a"(hint), "c"(extensions), "d"(timer));
}
#endif

