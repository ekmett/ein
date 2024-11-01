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
/// \defgroup initialization_attributes Initialization
///
///   \ingroup attributes
///
/// \{

/** \def ein_constinit
    \brief `constinit`

      \details

        `constinit` requires C++20, but `__attribute__((require_constant_initialization))`
        was available slightly earlier. */

#if __cpp_constinit
  #define ein_constinit constinit
#elif ein_has_attribute(require_constant_initialization)
  #define ein_constinit __attribute__((require_constant_initialization))
#else
  #define ein_constinit
#endif

/** \def ein_uninitialized
    \brief `[[clang::uninitialized]]`

      \details

        ensures a stack variable remains uninitialized regardless of
        `-ftrivial-auto-var-init=*` settings passed to the compiler */

#if ein_has_attribute(uninitialized)
  #define ein_uninitialized __attribute__((uninitialized))
#else
  #define ein_uninitialized
#endif

/** \def ein_reinitializes
    \brief `[[clang::reinitializes]]`

      \details

        Indicates to any uninitialised object state sanitizer
        that this restores an object to a fresh state independent
        of its previous state. */

#if __has_cpp_attribute(clang::reinitializes)
  #define ein_reinitializes [[clang::reinitializes]]
#else
  #define ein_reinitializes
#endif

/// \}
