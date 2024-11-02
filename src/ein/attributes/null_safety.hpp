#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \ingroup attributes */

#include "common.hpp"


///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup null_safety_attributes Null Safety
///
///   \ingroup attributes
///
/// \{

/** \def ein_returns_nonnull

      \brief `[[returns_nonnnull]]`

      \details

        The result of this function are guaranteed be non-null */

#if ein_has_attribute(returns_nonnull)
  #define ein_returns_nonnull __attribute__((returns_nonnull))
#else
  #define ein_returns_nonnull
#endif

/** \def ein_nonnull(...)

      \brief portable `[[nonnnull(...)]]`

      \details

        Indicates the selected (1-based) indexed arguments to this function
        must be non-null. Passing a null pointer to such an argument is
        undefined behavior. GCC-style. */

#if ein_has_attribute(nonnull)
  #define ein_nonnull(...) __attribute__((nonnull(__VA_ARGS__)))
#else
  #define ein_nonnull(...)
#endif

/** \def ein_Nonnull

      \brief `_Nonnull`

      \details

      can be applied to each * in an data type to indicate that argument should never be null.


    \def ein_Nullable

      \brief `_Nullable`

      \details

      can be applied to each * in an data type to indicate that argument might be null.


    \def ein_Null_unspecified

      \brief `_Null_unspecified`

      \details

      applied to each * in an data type to indicate that the nullability of it is unknown or complicated */

#ifdef __clang__
#define ein_Nonnull _Nonnull
#define ein_Nullable _Nullable
#define ein_Null_unspecified _Null_unspecified
#else
#define ein_Nonnull
#define ein_Nullable
#define ein_Null_unspecified
#endif

/// \}
