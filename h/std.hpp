#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \brief The C++ `std` library.

      \details

        Currently `clang-19` doesn't quite support `import std`.

        Capturing what portions of the `std` library we use for use
        in a precompiled header drastically reduces compile times.

        When `import std` is reliable, we can retire this header. */

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <concepts>
#include <cpuid.h>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <initializer_list>
#include <memory>
#include <string_view>
#include <type_traits>
