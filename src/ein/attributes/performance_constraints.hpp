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
/// \defgroup performance_constraint_attributes Performance Constraints
///
///   \details
///
///      - When assigning or otherwise converting to a function pointer of `nonblocking` or
///        `nonallocating` type, the source must also be a function or function pointer of that
///        type, unless it is a null pointer, i.e. the attributes should not be “spoofed”.
///        Conversions that remove the attributes are transparent and valid.
///
///      - An override of a `nonblocking` or `nonallocating` virtual method must also be declared
///        with that same attribute (or a stronger one.) An overriding method may add an attribute.
///
///      - A redeclaration of a `nonblocking` or `nonallocating` function must also be declared
///        with the same attribute (or a stronger one). A redeclaration may add an attribute.
///
///   \note
///
///     #ein_noalloc(x) and #ein_noblock(x) are necessitated by the inability to write a macro that
///     merely _may_ take an argument.
///
/// \ingroup attributes
///
/// \{

/**
    \def ein_allocating

      \brief Declares a function potentially allocates heap memory. Prevents inference of `nonallocating`.

    \def ein_blocking

      \brief Declares a function potentially blocks. Prevents inference of #ein_nonblocking.

    \def ein_nonblocking

      \brief Declares a function does not block.

    \def ein_noblock(x)

      \brief a `bool` parameterized version of #ein_nonblocking

      \details

        - #ein_noblock(false) behaves like #ein_blocking.

        - #ein_noblock(true) behavs like #ein_nonblocking

    \def ein_nonallocating

      \brief Declares a function does *not* allocate heap memory.

    \def ein_noalloc

      \brief a `bool` parameterized version of #ein_nonallocating

      \details

        - #ein_noalloc(false) behaves like #ein_allocating.

        - #ein_noalloc(true) behavs like #ein_nonallocating */

#if ein_has_attribute(nonblocking)
  #define ein_nonblocking [[clang::nonblocking]]
  #define ein_blocking [[clang::blocking]]
  #define ein_nonallocating [[clang::nonallocating]]
  #define ein_allocating [[clang::allocating]]
  #define ein_noalloc(x) [[clang::nonallocating(x)]]
  #define ein_noblock(x) [[clang::nonblocking(x)]]
#else
  #define ein_nonblocking
  #define ein_blocking
  #define ein_nonallocating
  #define ein_allocating
  #define ein_noalloc(__x)
  #define ein_noblock(__x)
#endif

/// \}
