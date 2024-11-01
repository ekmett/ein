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
/// \defgroup typestate_analysis Typestate Analysis
///
///
/// \brief Enabled in `clang` by passing `-fsanitize=consumed`
///
///   \ingroup attributes
///
/// \{

/** \def ein_consumable(x)

      \brief Each class that uses any typestate annotations must first be marked using this attribute.

      \details

        Failure to do so will result in a warning.
        This attribute accepts a single parameter that must be one of the following:
        "unknown", "consumed", or "unconsumed". */

#if ein_has_attribute(consumable)
  #define ein_consumable(__x) __attribute__((consumable(__x)))
#else
  #define ein_consumable(__x)
#endif

/** \def ein_callable_when(...)

      \details

        Use to indicate what states a method may be called in. Valid states
        are `"unconsumed"`, `"consumed"`, or `"unknown"`.
        Each argument to this attribute must be a quoted string. */

#if ein_has_attribute(callable_when)
  #define ein_callable_when(...) __attribute__((callable_when(__VA_ARGS__)))
#else
  #define ein_callable_when(...)
#endif

/** \def ein_param_typestate(...)

      \details

        This attribute specifies expectations about function parameters. Calls to a
        function with annotated parameters will issue a warning if the corresponding
        argument isn’t in the expected state. The attribute is also used to set the
        initial state of the parameter when analyzing the function’s body. */

#if ein_has_attribute(param_typestate)
  #define ein_param_typestate(...) __attribute__((param_typestate(__VA_ARGS__)))
#else
  #define ein_param_typestate(...)
#endif

/** \def ein_return_typestate(x)

      \details

        The `return_typestate` attribute can be applied to functions or parameters.

        When applied to a function the attribute specifies the state of the returned value.
        The function’s body is checked to ensure that it always returns a value in the
        specified state. On the caller side, values returned by the annotated function are
        initialized to the given state.

        When applied to a function parameter it modifies the state of an argument after a
        call to the function returns. The function’s body is checked to ensure that the
        parameter is in the expected state before returning. */

#if ein_has_attribute(return_typestate)
  #define ein_return_typestate(...) __attribute__((return_typestate(__VA_ARGS__)))
#else
  #define ein_return_typestate(...)
#endif

/** \def ein_moving

      \brief `[[return_typestate("consumed")]]` `[[param_typestate("unconsumed")]]`

      \details

        Short-hand for indicating that this argument was moved and is now `"consumed"`.
        Assumes it was `"unconsumed"` before. */

#define ein_moving ein_return_typestate("consumed") ein_param_typestate("unconsumed")

/** \def ein_set_typestate(x)

      \details

        Annotate methods that transition an object into a new state.

        The new state must be `"unconsumed"`, `"consumed"`, or `"unknown"`. */

#if ein_has_attribute(set_typestate)
  #define ein_set_typestate(...) __attribute__((set_typestate(__VA_ARGS__)))
#else
  #define ein_set_typestate(...)
#endif

/** \def ein_test_typestate(x)

      \details

        Use to indicate that a method returns true if the object is in the specified state.

        The state must be `"unconsumed"`, `"consumed"`, or `"unknown"`. */

#if ein_has_attribute(test_typestate)
  #define ein_test_typestate(...) __attribute__((test_typestate(__VA_ARGS__)))
#else
  #define ein_test_typestate
  #define ein_test_typestate(...)
#endif // ein_has_attribute(test_typestate)

/// \}
