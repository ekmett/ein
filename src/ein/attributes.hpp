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
/// \defgroup multiversioning_attributes Function Multi-Versioning
///
///   \brief attributes for function multiversioning
///
///   \details
///
///     Multi-versioned functions have their symbols resolved at linkage time by a function
///     that can look at the CPU and make linking decisions. These are theoretically great!
///
///     \warning
///
///       Unfortunately, they don't support `[[flatten]]` and they don't support function
///       templates as implemented at this time. When they do they'll become a powerful tool.
///
///   \ingroup attributes
///
/// \{

/** \def ein_target(x)

      \brief this indicates a required feature set for the current multiversioned function.

      \details

        Overloads will be resolved at load time by the linker based on detected cpu capabilities.
        Unfortunately this cannot be applied to templated functions (or ones that take `auto`) */

#define ein_target(x) __attribute__((target(x)))

/** \def ein_target_clones(...)

      \brief this indicates a required feature set for the current multiversioned function.

      \details

        Overloads will be resolved at load time. */

#define ein_target_clones(...) __attribute__((target_clones(_VA_ARGS_)))

/// \}
///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup calling_convention_attributes Calling Conventions
///
///   \brief alternative calling conventions
///
///   \ingroup attributes
///
/// \{

/** \def ein_hot

      \brief `[[hot]]`

      \details

        calling convention frequent inner loop calls */

#if ein_has_attribute(hot)
  #define ein_hot __attribute__((hot))
#else
  #define ein_hot
#endif

/** \def ein_cold

      \brief `[[cold]]`

      \details

        calling convention for very infrequent calls (e.g. initializers) */

#if ein_has_attribute(cold)
  #define ein_cold __attribute__((cold))
#else
  #define ein_cold
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
///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup thread_safety_attributes Thread Safety
///
///   \brief these are supported by `-Wthread-safety`
///
///   \ingroup attributes
///
/// \{

/** \def ein_no_thread_safety_analysis

    \details

    Use on a function declaration to specify that the thread safety analysis
    should not be run on that function. This attribute provides an escape hatch
    (e.g. for situations when it is difficult to annotate the locking policy). */

#if ein_has_attribute(no_thread_safety_analysis)
  #define ein_no_thread_safety_analysis __attribute__((no_thread_safety_analysis))
#else
  #define ein_no_thread_safety_analysis
#endif

/** \def ein_lockable

    \details

    Use on a class definition to specify that it has a
    lockable type (e.g. a Mutex class). This annotation is
    primarily used to check consistency. */

#if ein_has_attribute(lockable)
  #define ein_lockable __attribute__((lockable))
#else
  #define ein_lockable
#endif

/** \def ein_scoped_lockable

      \details

        Use on a class definition to specify that it has a
        “scoped” lockable type. Objects of this type will acquire the
        lock upon construction and release it upon going out of scope.
        This annotation is primarily used to check consistency. */

#if ein_has_attribute(scoped_lockable)
  #define ein_scoped_lockable __attribute__((scoped_lockable))
#else
  #define ein_scoped_lockable
#endif

/** \def ein_guarded_var

      \details

        Use on a variable declaration to specify that the variable
        must be accessed while holding some lock. */

#if ein_has_attribute(guarded_var)
  #define ein_guarded_var __attribute__((guarded_var))
#else
  #define ein_guarded_var
#endif

/** \def ein_pt_guarded_var

      \details

        Use on a pointer declaration to specify that the
        pointer must be dereferenced while holding some lock. */

#if ein_has_attribute(pt_guarded_var)
  #define ein_pt_guarded_var __attribute__((pt_guarded_var))
#else
  #define ein_pt_guarded_var
#endif

/** \def ein_guarded_by(l)

      \details

        Use on a variable declaration to specify that the
        variable must be accessed while holding lock \p l. */

#if ein_has_attribute(guarded_by)
  #define ein_guarded_by(x) __attribute__((guarded_by(x)))
#else
  #define ein_guarded_by(x)
#endif

/** \def ein_pt_guarded_by(l)

      \details

        Use on a pointer declaration to specify that the
        pointer must be dereferenced while holding lock \p l. */

#if ein_has_attribute(pt_guarded_by)
  #define ein_pt_guarded_by(x) __attribute__((pt_guarded_by(x)))
#else
  #define ein_pt_guarded_by(x)
#endif

/** \def ein_acquired_before(...)

      \details

        Use on a declaration of a lockable variable to
        specify that the lock must be acquired
        before all attribute arguments. Arguments must be
        lockable type, and there must be at least one argument. */

#if ein_has_attribute(acquired_before)
  #define ein_acquired_before(...) __attribute__((acquired_before(__VA_ARGS__)))
#else
  #define ein_acquired_before(...)
#endif

/** \def ein_acquired_after(...)

      \details

        Use on a declaration of a lockable variable to specify that
        the lock must be acquired after all attribute arguments.
        Arguments must be lockable type, and there must be at
        least one argument. */

#if ein_has_attribute(acquired_after)
  #define ein_acquired_after(...) __attribute__((acquired_after(__VA_ARGS__)))
#else
  #define ein_acquired_after(...)
#endif

/** \def ein_exclusive_lock_function(...)

      \details

        Use on a function declaration to specify that the function
        acquires all listed locks exclusively. This attribute takes
        zero or more arguments: either of lockable type or integers
        indexing into function parameters of lockable type. If no
        arguments are given, the acquired lock is implicitly that
        of the enclosing object. */

#if ein_has_attribute(exclusive_lock_function)
  #define ein_exclusive_lock_function(...) __attribute__((exclusive_lock_function(__VA_ARGS__)))
#else
  #define ein_exclusive_lock_function(...)
#endif

/** \def ein_shared_trylock_function(...)

      \details

        Use on a function declaration to specify that the function will try
        (without blocking) to acquire all listed locks, although the locks
        may be shared (e.g. read locks). This attribute takes one or more
        arguments. The first argument is an integer or boolean value
        specifying the return value of a successful lock acquisition. The
        remaining arugments are either of lockable type or integers
        indexing into function parameters of lockable type. If only one
        argument is given, the acquired lock is implicitly `this` of the
        enclosing object. */

#if ein_has_attribute(shared_trylock_function)
  #define ein_shared_trylock_function(...) __attribute__((shared_trylock_function(__VA_ARGS__)))
#else
  #define ein_shared_trylock_function(...)
#endif

/** \def ein_unlock_function(...)

      \details

        Use on a function declaration to specify that the function releases
        all listed locks. This attribute takes zero or more arguments: either
        of lockable type or integers indexing into function parameters of
        lockable type. If no arguments are given, the acquired lock is implicitly
        `this` of the enclosing object. */

#if ein_has_attribute(unlock_function)
  #define ein_unlock_function(...) __attribute__((unlock_function(__VA_ARGS__)))
#else
  #define ein_unlock_function(...)
#endif

/** \def ein_lock_returned(l)

      \details

        Use on a function declaration to
        specify that the function returns lock \p l (\p l must be of lockable
        type). This annotation is used to aid in resolving lock
        expressions. */

#if ein_has_attribute(lock_returned)
  #define ein_lock_returned(__x) __attribute__((lock_returned(__x)))
#else
  #define ein_lock_returned(__x)
#endif

/** \def ein_locks_excluded(...)

      \details

        Use on a function declaration to specify that the function must
        not be called with the listed locks. Arguments must be lockable type,
        and there must be at least one argument. */

#if ein_has_attribute(locks_excluded)
  #define ein_locks_excluded(...) __attribute__((locks_excluded(__VA_ARGS__)))
#else
  #define ein_locks_excluded(...)
#endif

/** \def ein_exclusive_locks_required(...)

      \details

        Use on a function declaration to specify that the function must
        be called while holding the listed exclusive locks. Arguments must
        be lockable type, and there must be at least one argument. */

#if ein_has_attribute(exclusive_locks_required)
  #define ein_exclusive_locks_required(...) __attribute__((exclusive_locks_required(__VA_ARGS__)))
#else
  #define ein_exclusive_locks_required(...)
#endif

/** \def ein_shared_locks_required(...)

      \details

        Use on a function declaration to specify that the function must be
        called while holding the listed shared locks. Arguments must be lockable type,
        and there must be at least one argument. */

#if ein_has_attribute(shared_locks_required)
  #define ein_shared_locks_required(...) __attribute__((shared_locks_required(__VA_ARGS__)))
#else
  #define ein_shared_locks_required(...)
#endif

/// \}
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
///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup aliasing_attributes Aliasing Control
///
///   \brief see also #ein_malloc
///
///   \ingroup attributes
///
/// \{
///

// NB: ein_restrict is also in this group, but is in config.hpp

/** \def ein_noalias

      \brief `__declspec(noalias)`

      \details

        Indicates a function only accesses data through pointer arguments

        Requires clang to be running with `-fdeclspec` or `-fms-extensions`
        but can aid alias analysis. */

#if ein_has_declspec_attribute(noalias)
  #define ein_noalias __declspec(noalias)
#else
  #define ein_noalias
#endif

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
/// \defgroup string_safety_attributes String Safety
///
///   \ingroup attributes
///
/// \{

/** \def ein_null_terminated_string_arg(N)

      \brief The \p N th argumnt is a null terminated string.

      \details

        This may be applied to a function that takes a `char *` or `char const *` at referenced argument \p N.

        It indicates that the passed argument must be a C-style null-terminated string.
        Specifically, the presence of the attribute implies that, if the pointer is non-null, the function may
        scan through the referenced buffer looking for the first zero byte. */

#if ein_has_attribute(null_terminated_string_arg)
  #define ein_null_terminated_string_arg(x) __attribute__((null_terminated_string_arg(x)))
#else
  #define ein_null_terminated_string_arg(x)
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
///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup control_flow_attributes Control Flow
///
///   \details
///
///     `[[assume(x)]]` also moraly belongs among these
///
///   \ingroup attributes
///
/// \{

/** \def ein_callback(...)

      \brief `[[callback(...)]]`

      \details

        Indicates the specified argument will be called back with the
        other named arguments. Complicated, see clang docs.
Allows better interprocedural analysis */

#if ein_has_attribute(callback)
  #define ein_callback(...) __attribute__((callback(__VA_ARGS__)))
#else
  #define ein_callback(...)
#endif

/** \def ein_noreturn

      \brief gcc-style `[[noreturn]]`

      \details

        Indicates the method does not return to the caller.

        <a href="https://www.youtube.com/watch?v=Dh994JcEfkI">A relevant testimonial by the Kingston Trio.</a> */

#if ein_has_attribute(noreturn)
#define ein_noreturn __attribute__((noreturn))
#else
#define ein_noreturn
#endif

/// \}
///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup null_safety_attributes Null Safety
///
///   \ingroup attributes
///
/// \{

/** \def ein_returns_nonnull

      \brief `[[returns_nonnnull]]`

      \details

        The result of this function are guaranteed be non-null */

#if ein_has_attribute(returns_nonnull)
  #define ein_returns_nonnull __attribute__((returns_nonnull))
#else
  #define ein_returns_nonnull
#endif

/** \def ein_nonnull(...)

      \brief portable `[[nonnnull(...)]]`

      \details

        Indicates the selected (1-based) indexed arguments to this function
        must be non-null. Passing a null pointer to such an argument is
        undefined behavior. GCC-style. */

#if ein_has_attribute(nonnull)
  #define ein_nonnull(...) __attribute__((nonnull(__VA_ARGS__)))
#else
  #define ein_nonnull(...)
#endif

/** \def ein_Nonnull

      \brief `_Nonnull`

      \details

      can be applied to each * in an data type to indicate that argument should never be null.


    \def ein_Nullable

      \brief `_Nullable`

      \details

      can be applied to each * in an data type to indicate that argument might be null.


    \def ein_Null_unspecified

      \brief `_Null_unspecified`

      \details

      applied to each * in an data type to indicate that the nullability of it is unknown or complicated */

#ifdef __clang__
#define ein_Nonnull _Nonnull
#define ein_Nullable _Nullable
#define ein_Null_unspecified _Null_unspecified
#else
#define ein_Nonnull
#define ein_Nullable
#define ein_Null_unspecified
#endif

/// \}
///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup cuda_attributes CUDA
///
///   \ingroup attributes
///
/// \{

/** \def ein_host

      \brief portable `__host__` for CUDA

      \details

      indicates the function should be available on the host

    \def ein_device

      \brief portable `__device__` for CUDA

      \details

      indicates the function should be available on the device

    \def ein_hd

      \brief applies both \ref ein_host and \ref ein_device

      \details

      most functions in the library should be defined this way

      may eventually add similar annotations for sycl and the like

    \def ein_global

      \brief portable `__global__` for CUDA

      \details

      indicates the function is a global entry point for a compute kernel */

#ifdef __CUDACC__
#define ein_host __host__
#define ein_device __device__
#define ein_global __global__
#else
#define ein_host
#define ein_device
#define ein_global
#endif

#define ein_hd ein_host ein_device

/// \}
///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup pragmas Emitting Pragmas
///
/// \{
// note: not ingroup attributes

/// \cond local
#define ein_pragma_(x) _Pragma (#x)
/// \endcond

/** \def ein_pragma
    \brief emit a \#pragma
    \hideinitializer */

#define ein_pragma(x) ein_pragma_(x)

/// \}
///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup loop_attributes Loops
///
///   \ingroup attributes
///
/// \{

/** \def ein_unroll

      \brief hint to unroll the following loop N times. (clang-specific)

    \def ein_unroll_full

      \brief hint to unroll the following loop fully. (clang-specific) */

#ifdef __clang__
  #define ein_unroll(N) ein_pragma(clang loop unroll_count(N))
  #define ein_unroll_full ein_pragma(clang loop unroll(full))
#else
  #define ein_unroll(N)
  #define ein_unroll_full
#endif

/// \}
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
///////////////////////////////////////////////////////////////////////////////////////////////
