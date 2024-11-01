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

#include "detection.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup alignment_attributes Memory Alignment
///
///   \brief see also #ein_alloc_align
///
///   \ingroup attributes
///
/// \{

/** \def ein_assume_aligned(x)

      \brief Indicates a function returns a pointer with alignment at least \p x */

#if ein_has_attribute(assume_aligned)
  #define ein_assume_aligned(x) __attribute__((assume_aligned(x)))
#else
  #define ein_assume_aligned(x)
#endif

/** \def ein_align_value(x)

      \brief `[[align_value(x)]]`

      \details

        the annotated pointer specified has alignment at least \p x */

#if ein_has_attribute(align_value)
  #define ein_align_value(x) __attribute__((align_value(x)))
#else
  #define ein_align_value(x)
#endif

/// \}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup allocation_attributes Memory Allocation
///
///   \ingroup attributes
///
///    \sa #ein_allocating, #ein_nonallocating, #ein_noalloc
///
/// \{

/** \def ein_malloc

      \brief `[[malloc]]`

      \details

        Indicates the returned memory does not alias with any other pointer */

#if ein_has_attribute(malloc)
  #define ein_malloc __attribute__((malloc))
#else
  #define ein_malloc
#endif

/** \def ein_alloc_align(N)

      \brief `[[alloc_align(N)]]`

      \details

        Indicates the 1-based argument number of a function that indicates the alignment of the returned result

        See #ein_malloc.

    */

#if ein_has_attribute(alloc_align)
  #define ein_alloc_align(N) __attribute__((alloc_align(N)))
#else
  #define ein_alloc_align(__N)
#endif

/** \def ein_alloc_size(N)

      \brief `[[alloc_size(x)]]`

      \details

        Arg # (1-based) of the attribute that tells you the size of the result in bytes.  */

#if ein_has_attribute(alloc_size)
  #define ein_alloc_size(N) __attribute__((alloc_size(N)))
#else
  #define ein_alloc_size(__N)
#endif

/// \}
