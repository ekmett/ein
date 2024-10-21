module;

#include <cxxabi.h>

export module ein.types;

import ein.memory;

namespace ein {

EIN(export,nodiscard,pure)
unique_str symbol(const char * name) noexcept {
  int status;
  return unique_str { abi::__cxa_demangle(name, nullptr, nullptr, &status) };
}

export template <typename T>
EIN(nodiscard,pure)
unique_str type() noexcept {
  return symbol(typeid(T).name());
}

EIN(export,nodiscard)
unique_str type_of(auto const & t) noexcept {
  return symbol(typeid(std::remove_cvref_t<decltype(t)>).name());
}

} // namespace ein
