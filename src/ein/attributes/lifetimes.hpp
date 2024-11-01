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
/// \defgroup lifetime_attributes Object Lifetimes
///
///   \ingroup attributes
///
/// \{

/** \def ein_lifetimebound

      \brief `[[lifetimebound]]`

      \details

        The argument must be kept alive as long as the result of the
        function is alive. Should be used for functions that return
        references or views into the target object.

        This is a strong hint that this object "owns" the result and
        is just letting you borrow it.

        _Editorial_:

        As a rule of thumb all pointer arguments should be analyzed
        to be either #ein_noescape or #ein_lifetimebound, and
        #ein_lifetimebound should be applied to all methods that
        return a self-reference */

#if ein_has_attribute(lifetimebound)
  #define ein_lifetimebound [[clang::lifetimebound]]
#else
  #define ein_lifetimebound
#endif

/** \def ein_noescape

      \brief portable `__attribute__((noescape))`

      \details

        argument is not captured by the function (rust-style borrow) */

#if ein_has_attribute(noescape)
  #define ein_noescape __attribute__((noescape))
#else
  #define ein_noescape
#endif

/** \def ein_nodiscard

      \brief C++17 `[[nodiscard]]`.

      \details

        The user should explicitly throw away the result rather than let it be silently discarded

        Note: Despite being already standard, this is used primarily to annotate the definition with
        a `[[nodiscard]]` qualifier in DOXYGEN.  */

#define ein_nodiscard [[nodiscard]]

/// \}
