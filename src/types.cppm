module;

#include <cxxabi.h>

using namespace std;

export module ein.types;

namespace ein::types {

/// \brief returns the unmangled name of a the type \p T
///
/// \details Ideally this would be fully `constexpr`, but i'm not building my own demangler.
///
/// \hideinitializer
/// \hideinlinesource
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

/// \brief returns the unmangled name of a the type of the (unused) argument passed to this function
/// \hideinlinesource \nodiscard \const
export /** \cond */ EIN(nodiscard,const) /** \endcond */
const string_view type_of(auto const & t) noexcept {
  return type<remove_cvref_t<decltype(t)>>();
}

/// \brief type \p T is one of the \p candidates
export template <typename T, typename ... candidates>
concept one_of_t = (std::is_same_v<T,candidates> || ... || false);

/// \brief type \p T is not one of the \p candidates
export template <typename T, typename ... candidates>
concept not_one_of_t = (!one_of_t<T,candidates...>);

/// \cond
template<typename T>
struct arg1;

template<typename Ret, typename Arg, typename ... Args>
struct arg1<Ret(Arg, Args...)> {
    using type = Arg;
};

template<typename Ret, typename Arg, typename ... Args>
struct arg1<Ret(*)(Arg, Args...)> : arg1<Ret(Arg,Args...)> {};
/// \endcond

/// \hideinitializer \hideinlinesource
export template <typename F>
using arg1_t = arg1<F>::type;

} // namespace ein

export namespace ein {
  using namespace ein::types;
}
