#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \brief an opinionated subset of clang/gcc attributes

      \details
        Historically, this header has been maintained across a number of my open source
        projects, where it has grown organically over time. Now, I'm rather aggressively
        pruning out historical attributes and the like. But the general philosophy is that
        if we might need it, we may as well keep it.

      \ingroup attributes */

/** \defgroup attributes Attributes

      \brief macros used to provide useful attributes

      \details

        There is a large number of attributes that can be applied to methods, functions,
        variables, structs, modules, etc. across all of the different compilers out there. */

#include "common.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup target_attributes Function Multi-Versioning
///
///   \brief attributes for function multiversioning
///
///   \details
///
///     Multi-versioned functions have their symbols resolved at linkage time by a function
///     that can look at the CPU and make linking decisions. These are theoretically great!
///
///     \warning
///
///       Unfortunately, they don't support `[[flatten]]` and they don't support function
///       templates as implemented at this time. When they do they'll become a powerful tool.
///
///   \ingroup attributes
///
/// \{

/** \def ein_target(x)

      \brief this indicates a required feature set for the current multiversioned function.

      \details

        Overloads will be resolved at load time by the linker based on detected cpu capabilities.
        Unfortunately this cannot be applied to templated functions (or ones that take `auto`) */

#define ein_target(x) __attribute__((target(x)))

/** \def ein_target_clones(...)

      \brief this indicates a required feature set for the current multiversioned function.

      \details

        Overloads will be resolved at load time. */

#define ein_target_clones(...) __attribute__((target_clones(_VA_ARGS_)))

/// \}
