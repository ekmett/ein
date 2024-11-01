#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \ingroup attributes */

#include "detection.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup purity_attributes Purity
///
///   \ingroup attributes
///
/// \{

/** \def ein_const
    \brief `[[const]]` is not `const`

    \details

        Indicates the result is entirely determined by the arguments
        and does not access main memory in any way, including accessing
        members of `this`

        This allows the compiler to easily elide/duplicate calls, because
        it doesn't need to consider aliasing at all when moving this
        function's body around.

        WHEN IN DOUBT USE `ein_pure`, which is a much safer way to annotate
        pure functional code */

#if ein_has_attribute(const)
  #define ein_const __attribute__((const))
#else
  #define ein_const
#endif

/** \def ein_pure

      \brief `[[pure]]`

      \details

        No side-effects other than return value, may inspect globals

        Allows the compiler to easily elide/duplicate calls.
        This can freely commute past other operations that can be shown
        not to affect the memory locations read. */

#if ein_has_attribute(pure)
  #define ein_pure __attribute__((pure))
#else
  #define ein_pure
#endif

/// \}
