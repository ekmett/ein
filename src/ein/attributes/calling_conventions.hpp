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
/// \defgroup calling_convention_attributes Calling Conventions
///
///   \brief alternative calling conventions
///
///   \ingroup attributes
///
/// \{

/** \def ein_hot

      \brief `[[hot]]`

      \details

        calling convention frequent inner loop calls */

#if ein_has_attribute(hot)
  #define ein_hot __attribute__((hot))
#else
  #define ein_hot
#endif

/** \def ein_cold

      \brief `[[cold]]`

      \details

        calling convention for very infrequent calls (e.g. initializers) */

#if ein_has_attribute(cold)
  #define ein_cold __attribute__((cold))
#else
  #define ein_cold
#endif

/// \}
