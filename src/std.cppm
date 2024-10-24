/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \brief polyfill missing standard library functions

      \details

        This file works around limitations of the installed libc++ / stdc++

      \ingroup std */


module;

#include "prelude.hpp"

/// \defgroup std std
export module ein.std;

//// \ingroup std
namespace std {

#ifndef __cpp_lib_forward_like
  namespace {
    template <typename T, typename U>
    using override_ref_t = conditional_t<is_rvalue_reference_v<T>, remove_reference_t<U> &&, U &>;

    template <typename T, typename U>
    using copy_const_t = conditional_t<is_const_v<remove_reference_t<T>>, U const, U>;

    template <typename T, typename U>
    using forward_like_t = override_ref_t<T &&, copy_const_t<T, remove_reference_t<U>>>;
  }

  /// \brief Returns a reference to x which has similar properties to T&&.
  ///
  /// The return type is determined as below:
  ///
  /// If `std::remove_reference_t<T>` is a `const`-qualified type, then the referenced type of the
  /// return type is `const std::remove_reference_t<U>`. Otherwise, the referenced type is `std::remove_reference_t<U>`.
  ///
  /// If `T&&` is an lvalue reference type, then the return type is also an lvalue reference type.
  ///
  /// Otherwise, the return type is an rvalue reference type.
  ///
  /// If T is not a referenceable type, the program is ill-formed.
  ///
  /// \tparam T the type to copy qualifications from
  /// \param x a value needs to be forwarded like type `T`
  /// \returns a reference to x of the type determined as above.
  ///
  /// \since 2022-07L __cpp_lib_forward_like
  /// \ingroup std
  export template <typename T>
    [[nodiscard]] constexpr
    auto forward_like(auto&& x) noexcept -> forward_like_t<T, decltype(x)> {
    return static_cast<forward_like_t<T, decltype(x)>>(x);
  }
#endif
}
