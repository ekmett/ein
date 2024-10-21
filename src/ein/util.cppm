module;
#include <type_traits>
export module ein.util;

export namespace ein {

#if __cplusplus >= 202002L
  using std::is_constant_evaluated;
#elif defined(__GNUC__) // defined for both GCC and clang
  constexpr bool is_constant_evaluated() noexcept {
    return __builtin_is_constant_evaluated();
  }
#endif

} // export namespace ein
