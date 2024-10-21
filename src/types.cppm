module;

#include <cxxabi.h>

using namespace std;

export module ein.types;

namespace ein::types {

export template <typename T>
EIN(nodiscard,const)
string_view type() noexcept {
  int status;
  size_t len = 0uz;
  static const char * result = abi::__cxa_demangle(typeid(T).name(), nullptr, &len, &status);
  return { result, len };
}


export EIN(nodiscard,const)
string_view type_of(auto const & t) noexcept {
  return type<remove_cvref_t<decltype(t)>>();
}

} // namespace ein

export namespace ein {
  using namespace ein::types;
}
