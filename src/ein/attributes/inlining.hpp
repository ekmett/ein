#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \brief an opinionated subset of clang/gcc attributes

      \ingroup attributes */

#include "detection.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup inlining_attributes Inlining Control
///
///   \brief attributes that control inlining behavior
///
///   \ingroup attributes
///
/// \{

/** \def ein_inline

      \brief `inline [[always_inline]]` */

#if ein_has_attribute(always_inline)
  #define ein_inline inline __attribute__((always_inline))
#elif defined _WIN32
  #define ein_inline __forceinline
#else
  #define ein_inline inline
#endif

/** \def ein_flatten

      \brief portable `[[flatten]]`

      \details

        the compiler should inline recursively aggressively under
        this definition. use with care, good for expression templates */

#if ein_has_attribute(flatten)
  #define ein_flatten __attribute__((flatten))
#else
  #define ein_flatten
#endif

/** \def ein_artificial

      \brief `[[artificial]]`.

      \details

        The debugger should not single-step into this function.
        Treat it as atomic and associate it with the debug information for the use site instead.  */

#if ein_has_attribute(artificial)
  #define ein_artificial __attribute__((artificial))
#elif ein_has_attribute(__artificial__)
  #define ein_artificial __attribute__((__artificial__))
#else
  #define ein_artificial
  #warning "[[artificial]] is not supported"
#endif

/** \def ein_noinline

      \brief `[[noinline]]`

      \details

        Indicates that the inliner should avoid inlining this function at usage sites */

#if ein_has_attribute(noinline)
  #define ein_noinline __attribute__((noinline))
#else
  #define ein_noinline
#endif

/** \def ein_optnone

      \brief `[[optnone]]`

      \details

        Used to indicate that regardless of optimization level, you shouldn't optimize this one function.
        Useful for local debugging when you can't disable optimization for the entire build. (e.g. ours!) */

#if ein_has_attribute(optnone)
  #define ein_optnone __attribute__((optnone))
#else
  #define ein_optnone
#endif

/// \}
