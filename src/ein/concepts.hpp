#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

    \def ein_ensures(concept,x)

    \brief statically checks that a concept is satisfied by the type of an expression

    \details

      This is expected most often to be used inline in a return statement:

      \code{.cpp}
      auto foo(integral auto x, integral auto y) {
        return ein_ensures(integral, x + y);
      }
      \endcode

    \param concept the name of a C++ concept to check
    \param x the expression to check returns a result that satisfies it
    \returns x */

#include <concepts>

#define ein_ensures(concept, x) \
  ( \
    []<typename T> static constexpr noexcept { \
      static_assert(concept<T>, #concept " not satisfied"); \
    }.template operator()<decltype(x)>(), \
    (x) \
  )
