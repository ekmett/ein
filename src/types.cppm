/// \file
/// \ingroup types
/// \license
/// SPDX-FileType: Source
/// SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
/// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
/// \endlicense
module;

#ifndef EIN_PCH
#include "prelude.hpp"
#endif

#include <cxxabi.h>

using namespace std;

/// \ingroup types
export module ein.types;

export namespace ein {
/// \defgroup types types
/// \{

/// \brief returns the unmangled name of a the type \p T
///
/// \details Ideally this would be fully `constexpr`, but i'm not building my own demangler.
///
/// \hideinitializer
/// \hideinlinesource
template <typename T>
const string_view type = [] EIN(nodiscard,const) static noexcept -> string_view {
  static const string_view body = [] EIN(nodiscard,const) static noexcept -> string_view {
    int status;
    size_t len = 0uz;
    const char * str = abi::__cxa_demangle(typeid(T).name(), nullptr, &len, &status);
    return {str, len};
  }();
  return body;
}();

/// \brief returns the unmangled name of a the type of the (unused) argument passed to this function
/// \hideinlinesource \nodiscard \const \cond
EIN(nodiscard,const) /// \endcond
const string_view type_of(auto const & t) noexcept {
  return type<remove_cvref_t<decltype(t)>>();
}

/// \brief type \p T is one of the \p candidates
template <typename T, typename ... candidates>
concept one_of_t = (std::is_same_v<T,candidates> || ... || false);

/// \brief type \p T is not one of the \p candidates
template <typename T, typename ... candidates>
concept not_one_of_t = (!one_of_t<T,candidates...>);
/// \}
}