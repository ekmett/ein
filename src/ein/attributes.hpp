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
        if we might need it, we may as well keep it. */

/** \defgroup attributes Attributes

      \brief macros used to provide useful attributes

      \details

        There is a large number of attributes that can be applied to methods, functions,
        variables, structs, modules, etc. across all of the different compilers out there. */

#include "attributes/detection.hpp"
#include "attributes/inlining.hpp"
#include "attributes/targets.hpp"
#include "attributes/calling_conventions.hpp"
#include "attributes/linkage.hpp"
#include "attributes/typestate.hpp"
#include "attributes/thread_safety.hpp"
#include "attributes/handles.hpp"
#include "attributes/initialization.hpp"
#include "attributes/purity.hpp"
#include "attributes/memory.hpp"
#include "attributes/strings.hpp"
#include "attributes/lifetimes.hpp"
#include "attributes/control_flow.hpp"
#include "attributes/null_safety.hpp"
#include "attributes/cuda.hpp"
#include "attributes/performance_constraints.hpp"
