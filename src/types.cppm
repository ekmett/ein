module;

#include <cxxabi.h>

using namespace std;

export module ein.types;

namespace ein::types {

export template <typename T>
const string_view type = []() noexcept -> string_view {
  // yo dawg
  static const string_view bod = [&]() noexcept -> string_view {
    int status;
    size_t len = 0uz;
    const char * str = abi::__cxa_demangle(typeid(T).name(), nullptr, &len, &status);
    return {str, len};
  }();
}();

export EIN(nodiscard,const)
string_view type_of(auto const & t) noexcept {
  return type<remove_cvref_t<decltype(t)>>();
}

template <typename T, typename ... candidates>
concept one_of_t = (std::is_same_v<T,candidates> || ... || false);

template <typename T, typename ... candidates>
concept not_one_of_t = (!one_of_t<T,candidates...>);

} // namespace ein

export namespace ein {
  using namespace ein::types;
}


