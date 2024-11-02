#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \ingroup attributes */


///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup attribute_detection Attribute Detection
///
///   \brief macros for determining supported attributes
///
/// \ingroup attributes
/// \{

/** \def ein_has_attribute(x)

      \brief `__has_attribute(x)`

      \details

      Used as the primary means for detecting advanced annotation features */

#ifdef __has_attribute
  #define ein_has_attribute(x) __has_attribute(x)
#else
  #define ein_has_attribute(__x) 0
#endif

/** \def ein_has_declspec_attribute(__x)

      \brief portable `__has_declspec_attribute(__x)`

      \details

        Defaults to 1 on MSVC, where we can assume `__declspec(x)`
        is well supported.

        Uses the `__has_declspec_attribute(x)` implementation in Clang

        Defaults to 0 otherwise. */

#if defined(_MSC_VER)
  #define ein_has_declspec_attribute(__x) 1
#elif defined(EIN_USE_DECLSPEC) && defined(__has_declspec_attribute)
  #define ein_has_declspec_attribute(x) __has_declspec_attribute(x)
#else
  #define ein_has_declspec_attribute(__x) 0
#endif

/// \}
///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup purity_attributes Purity
///
///   \ingroup attributes
///
/// \{

/** \def ein_const
    \brief `[[const]]` is not `const`

    \details

        Indicates the result is entirely determined by the arguments
        and does not access main memory in any way, including accessing
        members of `this`

        This allows the compiler to easily elide/duplicate calls, because
        it doesn't need to consider aliasing at all when moving this
        function's body around.

        WHEN IN DOUBT USE `ein_pure`, which is a much safer way to annotate
        pure functional code */

#if ein_has_attribute(const)
  #define ein_const __attribute__((const))
#else
  #define ein_const
#endif

/** \def ein_pure

      \brief `[[pure]]`

      \details

        No side-effects other than return value, may inspect globals

        Allows the compiler to easily elide/duplicate calls.
        This can freely commute past other operations that can be shown
        not to affect the memory locations read. */

#if ein_has_attribute(pure)
  #define ein_pure __attribute__((pure))
#else
  #define ein_pure
#endif

/// \}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup lifetime_attributes Object Lifetimes
///
///   \ingroup attributes
///
/// \{

/** \def ein_lifetimebound

      \brief `[[lifetimebound]]`

      \details

        The argument must be kept alive as long as the result of the
        function is alive. Should be used for functions that return
        references or views into the target object.

        This is a strong hint that this object "owns" the result and
        is just letting you borrow it.

        _Editorial_:

        As a rule of thumb all pointer arguments should be analyzed
        to be either #ein_noescape or #ein_lifetimebound, and
        #ein_lifetimebound should be applied to all methods that
        return a self-reference */

#if ein_has_attribute(lifetimebound)
  #define ein_lifetimebound [[clang::lifetimebound]]
#else
  #define ein_lifetimebound
#endif

/** \def ein_noescape

      \brief portable `__attribute__((noescape))`

      \details

        argument is not captured by the function (rust-style borrow) */

#if ein_has_attribute(noescape)
  #define ein_noescape __attribute__((noescape))
#else
  #define ein_noescape
#endif

/** \def ein_nodiscard

      \brief C++17 `[[nodiscard]]`.

      \details

        The user should explicitly throw away the result rather than let it be silently discarded

        Note: Despite being already standard, this is used primarily to annotate the definition with
        a `[[nodiscard]]` qualifier in DOXYGEN.  */

#define ein_nodiscard [[nodiscard]]

/// \}


///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup inlining_attributes Inlining Control
///
///   \brief attributes that control inlining behavior
///
///   \ingroup attributes
///
/// \{

/** \def ein_inline

      \brief `inline [[always_inline]]` */

#if ein_has_attribute(always_inline)
  #define ein_inline inline __attribute__((always_inline))
#elif defined _WIN32
  #define ein_inline __forceinline
#else
  #define ein_inline inline
#endif

/** \def ein_flatten

      \brief portable `[[flatten]]`

      \details

        the compiler should inline recursively aggressively under
        this definition. use with care, good for expression templates */

#if ein_has_attribute(flatten)
  #define ein_flatten __attribute__((flatten))
#else
  #define ein_flatten
#endif

/** \def ein_artificial

      \brief `[[artificial]]`.

      \details

        The debugger should not single-step into this function.
        Treat it as atomic and associate it with the debug information for the use site instead.  */

#if ein_has_attribute(artificial)
  #define ein_artificial __attribute__((artificial))
#elif ein_has_attribute(__artificial__)
  #define ein_artificial __attribute__((__artificial__))
#else
  #define ein_artificial
  #warning "[[artificial]] is not supported"
#endif

/** \def ein_noinline

      \brief `[[noinline]]`

      \details

        Indicates that the inliner should avoid inlining this function at usage sites */

#if ein_has_attribute(noinline)
  #define ein_noinline __attribute__((noinline))
#else
  #define ein_noinline
#endif

/** \def ein_optnone

      \brief `[[optnone]]`

      \details

        Used to indicate that regardless of optimization level, you shouldn't optimize this one function.
        Useful for local debugging when you can't disable optimization for the entire build. (e.g. ours!) */

#if ein_has_attribute(optnone)
  #define ein_optnone __attribute__((optnone))
#else
  #define ein_optnone
#endif

/// \}
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

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup initialization_attributes Initialization
///
///   \ingroup attributes
///
/// \{

/** \def ein_constinit
    \brief `constinit`

      \details

        `constinit` requires C++20, but `__attribute__((require_constant_initialization))`
        was available slightly earlier. */

#if __cpp_constinit
  #define ein_constinit constinit
#elif ein_has_attribute(require_constant_initialization)
  #define ein_constinit __attribute__((require_constant_initialization))
#else
  #define ein_constinit
#endif

/** \def ein_uninitialized
    \brief `[[clang::uninitialized]]`

      \details

        ensures a stack variable remains uninitialized regardless of
        `-ftrivial-auto-var-init=*` settings passed to the compiler */

#if ein_has_attribute(uninitialized)
  #define ein_uninitialized __attribute__((uninitialized))
#else
  #define ein_uninitialized
#endif

/** \def ein_reinitializes
    \brief `[[clang::reinitializes]]`

      \details

        Indicates to any uninitialised object state sanitizer
        that this restores an object to a fresh state independent
        of its previous state. */

#if __has_cpp_attribute(clang::reinitializes)
  #define ein_reinitializes [[clang::reinitializes]]
#else
  #define ein_reinitializes
#endif

/// \}
