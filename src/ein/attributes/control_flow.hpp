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
/// \defgroup control_flow_attributes Control Flow
///
///   \details
///
///     `[[assume(x)]]` also moraly belongs among these
///
///   \ingroup attributes
///
/// \{

/** \def ein_callback(...)

      \brief `[[callback(...)]]`

      \details

        Indicates the specified argument will be called back with the
        other named arguments. Complicated, see clang docs.
Allows better interprocedural analysis */

#if ein_has_attribute(callback)
  #define ein_callback(...) __attribute__((callback(__VA_ARGS__)))
#else
  #define ein_callback(...)
#endif

/** \def ein_noreturn

      \brief gcc-style `[[noreturn]]`

      \details

        Indicates the method does not return to the caller.

        <a href="https://www.youtube.com/watch?v=Dh994JcEfkI">A relevant testimonial by the Kingston Trio.</a> */

#if ein_has_attribute(noreturn)
#define ein_noreturn __attribute__((noreturn))
#else
#define ein_noreturn
#endif

/// \}
///////////////////////////////////////////////////////////////////////////////////////////////
