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
/// \defgroup handle_attributes Handles
///
///   \brief Handles are a way to identify resources like files, sockets, and processes.
///
///   \details
///
///     They are more opaque than pointers and widely used in system programming. They have similar
///     risks such as never releasing a resource associated with a handle, attempting to use a
///     handle that was already released, or trying to release a handle twice.
///
///     Using the annotations below it is possible to make the ownership of the handles clear:
///     whose responsibility is to release them. They can also aid static analysis tools to find bugs.
///
///   \ingroup attributes
///
/// \{                                                                                             */


/** \def ein_acquire_handle(x)

      \brief indicates this method returns an acquired resource

      \details

        Names its type with a tag.

        If applied to a method it refers to the function result
        If applied to an argument it indicates that argument is an out-parameter providing the value */

#if __has_cpp_attribute(acquire_handle)
  #define ein_acquire_handle(__x) [[clang::acquire_handle(__x)]]
#else
  #define ein_acquire_handle(__x)
#endif

/** \def ein_release_handle(x)

      \brief Indicates this argument is an acquired resource that is being released with a tag */

#if __has_cpp_attribute(release_handle)
  #define ein_release_handle(__x) [[clang::release_handle(__x)]]
#else
  #define ein_release_handle(__x)
#endif

/** \def ein_use_handle(x)

      \brief indicates this argument is a use of a resource, but does not release it. */

#if __has_cpp_attribute(use_handle)
  #define ein_use_handle(__x) [[clang::use_handle(__x)]]
#else
  #define ein_use_handle(__x)
#endif

/// \}
