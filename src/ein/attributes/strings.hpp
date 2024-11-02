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
/// \defgroup string_safety_attributes String Safety
///
///   \ingroup attributes
///
/// \{

/** \def ein_null_terminated_string_arg(N)

      \brief The \p N th argumnt is a null terminated string.

      \details

        This may be applied to a function that takes a `char *` or `char const *` at referenced argument \p N.

        It indicates that the passed argument must be a C-style null-terminated string.
        Specifically, the presence of the attribute implies that, if the pointer is non-null, the function may
        scan through the referenced buffer looking for the first zero byte. */

#if ein_has_attribute(null_terminated_string_arg)
  #define ein_null_terminated_string_arg(x) __attribute__((null_terminated_string_arg(x)))
#else
  #define ein_null_terminated_string_arg(x)
#endif

/// \}
