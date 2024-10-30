#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \details See \ref pch_cmake_vs_clangd */

#include "std.hpp"         // c++ standard library
#include "intrinsics.hpp"  // platform intrinsics
#include "third-party.hpp" // third-party libraries we need
#include "attributes.hpp"  // our custom attribute wrappers
#include "assert.hpp"      // assertions
#include "concepts.hpp"    // concepts as type-level postconditions
#include "posix.hpp"       // posix functions e.g. `get_pid`
