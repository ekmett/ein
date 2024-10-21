module;

#include <cxxabi.h>

using namespace std;

export module ein.types;

namespace ein::types {

export template <typename T>
const string_view type = [] EIN(nodiscard,const) noexcept -> string_view {
  static const string_view body = [] EIN(nodiscard,const) noexcept -> string_view {
    int status;
    size_t len = 0uz;
    const char * str = abi::__cxa_demangle(typeid(T).name(), nullptr, &len, &status);
    return {str, len};
  }();
  return body;
}();

export EIN(nodiscard,const)
const string_view type_of(auto const & t) noexcept {
  return type<remove_cvref_t<decltype(t)>>();
}

export template <typename T, typename ... candidates>
concept one_of_t = (std::is_same_v<T,candidates> || ... || false);

export template <typename T, typename ... candidates>
concept not_one_of_t = (!one_of_t<T,candidates...>);

} // namespace ein

export namespace ein {
  using namespace ein::types;
}
