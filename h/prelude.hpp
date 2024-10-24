#pragma once

/** \file

      \ingroup headers

      \brief All c++ headers.

      \details

        When using CMake it wants to control the assembly of your precompiled
        header. To that end, it makes it awkward when you edit any file from
        your project that is included by the precompiled header that isn't
        named in your CMakeLists.txt file.

        We let CMake build its own precompiled header by naming the same
        headers listed below in the precompiled headers in src/CMakeLists.txt

        Clangd on the other seems to have issues with using precompiled headers
        and c++ modules. Notably, it expects `module` to be the absolute first
        thing in a source file, but I have no way to get "ahead" of the PCH.

        So for now, I'm exploring being able to build with and without precompiled
        headers. Since this is in the PCH header set you can import this header
        once in the top of each source file and let it be filtered out via
        `#pragma once` and your code will work either way with no import changes.

        Eventually, if we want the non-PCH mode to be as fast as possible, it should
        adopt a 'least import' practice, but it only exists to satisfy `clangd`.

        I prefer the clarity of the code with no headers cluttering the top of each
        file.

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

    \defgroup headers Headers
    \defgroup macros Macros */

#include "std.hpp"         // c++ standard library
#include "intrinsics.hpp"  // platform intrinsics
#include "third-party.hpp" // third-party libraries we need
#include "attributes.hpp"  // our custom attribute wrappers
#include "assert.hpp"      // assertions
