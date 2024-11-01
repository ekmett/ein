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
/// \defgroup linkage_attributes Linkage Control
///
///   \brief see \ref instantiation_attributes for more.
///
///   \ingroup attributes
///
/// \{

/** \def ein_weak

      \brief `[[weak]]`

      \details

        used to generate a symbol that can be easily overriden by the linker. */

#if ein_has_attribute(weak)
  #define ein_weak __attribute__((weak))
#else
  #define ein_weak
#endif

/** \def ein_internal_linkage

      \brief `[[internal_linkage]]`

      \details

        Changes the linkage type of the declaration to internal. This is similar to
        C-style static, but can be used on classes and class methods. When applied
        to a class definition, this attribute affects all methods and static data
        members of that class. This can be used to contain the ABI of a C++ library
        by excluding unwanted class methods from the export tables.

        \warning probably subsumed by ein_visibility("hidden") */

#if ein_has_attribute(internal_linkage)
  #define ein_internal_linkage __attribute__((internal_linkage))
#else
  #define ein_internal_linkage
#endif

/** \def ein_preferred_name(x)

      \brief portable `[[clang::preferred_name(x)]]` annotations

      \details

        Can be applied to a template struct to provide better names
        for some concrete instantiations */

#if __has_cpp_attribute(clang::preferred_name)
  #define ein_preferred_name(x) [[clang::preferred_name(__x)]]
#else
  #define ein_preferred_name(__x)
#endif

/// \}
///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup instantiation_attributes Explicit Template Instantiation
///
///   \brief attributes that can control explicit template instantiation
///
///   \ingroup linkage_attributes
///
/// \{

/** \def ein_visibility

      \brief `[[visibility(x)]]`

      \details

        Primarily used for `[[visibility("hidden")]]` to disable inclusion of the symbol in the resulting library */

#if ein_has_attribute(__visibility__)
  #define ein_visibility(...) __attribute__((__visibility__(__VA_ARGS__)))
#else
  #define ein_visibility(...)
  #warning "[[visibility]] is not supported"
#endif

/** \def ein_exclude_from_explicit_instantiation

      \brief exclude this member from explicit template instantiation.

      \details

        Usually paired with #ein_visibility`("hidden")`, see #ein_hidden */

#if ein_has_attribute(exclude_from_explicit_instantiation)
  #define ein_exclude_from_explicit_instantiation __attribute__((exclude_from_explicit_instantiation))
#else
  #define ein_exclude_from_explicit_instantiation
  #warning "[[exclude_from_explicit_instantiation]] is not supported"
#endif

/** \def ein_hidden

      \brief `[[visibility("hidden")]] [[exclude_from_explicit_instantiations]]`

      \details

        Use to exclude from both having to compute in explicit template instantiations
        and expecting it to be stored. This means that the resulting member is safely
        delayed to the usage site, despite the template being explicitly otherwise
        instantiated. Useful for `#ein_inline` and the general sort of SIMD intrinsic
        wrappers where there's no point in emitting a function body that will never be
        called, and will just bloat executable size. */

#define ein_hidden ein_visibility("hidden") ein_exclude_from_explicit_instantiation

/// \}
