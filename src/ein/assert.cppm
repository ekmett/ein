module;

#include "ein/attributes.hpp"
#include <type_traits> // std::false_type

#ifdef __cpp_lib_unreachable
#include <utility> // unreachable
#endif

export module ein.assert;

namespace ein {

template <typename T>
struct no_ : std::false_type {
};


export template <typename T>
constexpr bool no = no_<T>::value;

EIN(export,inline)
void assume(bool b) noexcept {
#ifdef __cpp_lib_unreachable
  if (!b) std::unreachable();
#else
  if (!b) __builtin_unreachable();
#endif
}

} // namespace ein
