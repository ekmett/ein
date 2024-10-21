module;

#include <type_traits> // std::false_type
#ifdef __cpp_lib_unreachable
#include <utility> // unreachable
#endif

export module ein.assert;

namespace ein {

/// \private
template <typename T>
struct no_ : std::false_type {
};

/// \brief lazy enough `false` that static_assert won't trigger unless it means it
///
/// `no<T>` is useful to enable static_assert to check that
/// type arguments actually match a partial template specialization.
///
/// ```
/// template <typename T>
/// struct specialize_me {
///      static_assert(no<T>,"you must specialize this");
/// };
/// ```
///
/// now you can implement several specializations
///
/// ```
/// template <typename X>
/// struct specialize_me<std::atomic<X>> {
///
/// };
/// ```
///
/// no<T> is required, because if you replace it with `false`
/// the compiler will eagerly complain at you violating the constraint
///
/// this works because the user _could_ partially specialize the kind of secret
/// `no_<T>` template at some type T, so it is _possible_ for the answer to become
/// `true`, letting us delay the check until you instantiate the template with
/// bad arguments.

export template <typename T>
constexpr bool no = no_<T>::value;

export void assume(bool b) noexcept {
#ifdef __cpp_lib_unreachable
  if (!b) std::unreachable();
#else
  if (!b) __builtin_unreachable();
#endif
}

} // namespace ein
