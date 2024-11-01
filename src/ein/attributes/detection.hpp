#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \brief an opinionated subset of clang/gcc attributes

      \ingroup attributes */

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup attribute_detection Attribute Detection
///
///   \brief macros for determining supported attributes
///
/// \ingroup attributes
/// \{

/** \def ein_has_attribute(x)

      \brief `__has_attribute(x)`

      \details

      Used as the primary means for detecting advanced annotation features */

#ifdef __has_attribute
  #define ein_has_attribute(x) __has_attribute(x)
#else
  #define ein_has_attribute(__x) 0
#endif

/** \def ein_has_declspec_attribute(__x)

      \brief portable `__has_declspec_attribute(__x)`

      \details

        Defaults to 1 on MSVC, where we can assume `__declspec(x)`
        is well supported.

        Uses the `__has_declspec_attribute(x)` implementation in Clang

        Defaults to 0 otherwise. */

#if defined(_MSC_VER)
  #define ein_has_declspec_attribute(__x) 1
#elif defined(EIN_USE_DECLSPEC) && defined(__has_declspec_attribute)
  #define ein_has_declspec_attribute(x) __has_declspec_attribute(x)
#else
  #define ein_has_declspec_attribute(__x) 0
#endif

/// \}
