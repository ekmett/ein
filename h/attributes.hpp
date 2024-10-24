#pragma once

/**
  \file
  \brief defines used to indicate useful attributes for the compiler
  \ingroup macros
  \license
  SPDX-FileType: Source
  SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
  SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
  \endlicense
*/

/// \ingroup macros
/// \{

#include "map.hpp"

/** \def EIN(...)
    \brief
    Applies the prefix `ein_` syntactically to all arguments. Used to apply many attributes to definitions.
    \details
    This lets you write EIN(hd,inline) vs. \ref ein_hd \ref ein_inline
*/
#if !defined(DOXYGEN) && !defined(SWIG)
#define EIN(...) EIN_MAP(ein_,__VA_ARGS__)
#else
#define EIN(...)
#endif

/// \cond
#define ein_pragma_(x) _Pragma (#x)
/// \endcond

/// \def ein_pragma
/// \brief emit a \#pragma
/// \hideinitializer
#define ein_pragma(x) ein_pragma_(x)

/// \def ein_export
/// \brief export from the current module
#define ein_export export

/// \def ein_message
/// \brief emit a warning if EIN_VERBOSE is set during compile
#ifdef EIN_VERBOSE
#define ein_message(n) ein_pragma(message(#n))
#else
#define ein_message(n)
#endif

/// \def ein_has_attribute(x)
/// \brief portable `__has_attribute(x)`
#ifdef __has_attribute
#define ein_has_attribute(x) __has_attribute(x)
#else
#define ein_has_attribute(x) 0
#endif

/// \def ein_has_declspec_attribute(__x)
/// \brief portable `__has_declspec_attribute(__x)`
#if defined(_MSC_VER)
#define ein_has_declspec_attribute(x) 1
#elif defined(EIN_USE_DECLSPEC) && defined(__has_declspec_attribute)
#define ein_has_declspec_attribute(x) __has_declspec_attribute(x)
#else
#define ein_has_declspec_attribute(x) 0
#endif

/// \def ein_inline
/// \brief portable `inline __forceinline` or `inline __attribute__((always_inline))`
#if ein_has_attribute(always_inline)
#define ein_inline inline __attribute__((always_inline))
#elif defined _WIN32
#define ein_inline __forceinline
#else
#define ein_inline inline
#endif

/// \def ein_artificial
/// \brief portable `__attribute__((artificial))`. The debugger should not single-step into this function.
/// Treat it as atomic and associate it with the debug information for the use site instead.
#if ein_has_attribute(artificial)
#define ein_artificial __attribute__((artificial))
#elif ein_has_attribute(__artificial__)
#define ein_artificial __attribute__((__artificial__))
#else
#define ein_artificial
ein_message("no artificial")
#endif

/// \def ein_visibility
/// \brief portable `__attribute__((visibility(x)))`, primarily used for `visibility("hidden")` to
/// disable inclusion in the library
#if ein_has_attribute(__visibility__)
#define ein_visibility(...) __attribute__((__visibility__(__VA_ARGS__)))
#else
#define ein_visibility(...)
ein_message("no visibility")
#endif

/// \def ein_exclude_from_explicit_instantiation
/// \brief exclude this member from explicit template instantiation. paired with ein_hidden
#if ein_has_attribute(exclude_from_explicit_instantiation)
#define ein_exclude_from_explicit_instantiation __attribute__((exclude_from_explicit_instantiation))
#else
#define ein_exclude_from_explicit_instantiation
ein_message("no exclude_from_explicit_instantiation")
#endif

/// \def ein_hidden
/// \brief this member is excluded from both explicit instantiation and external linkage
/// \details
/// expands to `__attribute__((visibility("hidden"))` `__attribute__((exclude_from_explicit_instantiation))`
#define ein_hidden ein_visibility("hidden") ein_exclude_from_explicit_instantiation

/// \def ein_target(x)
/// \brief this indicates a required feature set for the current multiversioned function.
/// \details overloads will be resolved at load time
#define ein_target(x) __attribute__((target(x)))

/// \def ein_target_clones(...)
/// \brief this indicates a required feature set for the current multiversioned function.
/// \details overloads will be resolved at load time
#define ein_target_clones(...) __attribute__((target_clones(_VA_ARGS_)))

/// \def ein_noinline
/// \brief portable `__attribute__((noinline))`

#if ein_has_attribute(noinline)
#define ein_noinline __attribute__((noinline))
#else
#define ein_noinline
ein_message("no noinline")
#endif

/// \def ein_optnone
/// \brief portable `__attribute__((optnone))`
/// Used to indicate that regardless of optimization level, you shouldn't optimize this one function.
/// Useful for local debugging when you can't disable optimization for the entire build. (e.g. ours!)
#if ein_has_attribute(optnone)
#define ein_optnone __attribute__((optnone))
#else
#define ein_optnone
ein_message("no optnone")
#endif

/// \def ein_hot
/// \brief portable `__attribute__((hot))`
/// calling convention frequent inner loop calls
#if ein_has_attribute(hot)
#define ein_hot __attribute__((hot))
#else
#define ein_hot
ein_message("no hot")
#endif

/// \def ein_cold
/// \brief portable `__attribute__((cold))`
/// calling convention very infrequent calls (initializers and the like)
#if ein_has_attribute(cold)
#define ein_cold __attribute__((cold))
#else
#define ein_cold
ein_message("no cold")
#endif

/// \def ein_weak
/// \brief __attribute__((weak))
/// used to generate a symbol that can be easily overriden by the
/// linker. intended for 'fake pos' support
#if ein_has_attribute(weak)
#  define ein_weak __attribute__((weak))
#elif defined _WIN32
#define ein_weak
#  else
#  define ein_weak // for doxygen
#  error "weak symbols not supported"
#endif // ein_has_attribute(weak)

#if ein_has_attribute(internal_linkage)
#define ein_internal_linkage __attribute__((internal_linkage))
#elif defined _WIN32
#define ein_internal_linkage
#else
ein_message("no internal_linkage")
#endif

// state analysis
// ==============

/// \def ein_callable_when(...)
/// Use __attribute__((callable_when(...))) to indicate what states a method may be called in. Valid states are unconsumed, consumed, or unknown. Each argument to this attribute must be a quoted string. E.g.:
#if ein_has_attribute(callable_when)
#define ein_callable_when(...) __attribute__((callable_when(__VA_ARGS__)))
#else
#define ein_callable_when(...)
#endif

/// \def ein_consumable(x)
/// Each class that uses any of the typestate annotations must first be marked using the consumable attribute. Failure to do so will result in a warning.
/// This attribute accepts a single parameter that must be one of the following: unknown, consumed, or unconsumed.
#if ein_has_attribute(consumable)
#define ein_consumable(__x) __attribute__((consumable(__x)))
#else
#define ein_consumable(__x)
#endif

/// \def ein_param_typestate(...)
/// This attribute specifies expectations about function parameters. Calls to an function with annotated parameters will issue a warning if the corresponding argument isn’t in the expected state. The attribute is also used to set the initial state of the parameter when analyzing the function’s body.
#if ein_has_attribute(param_typestate)
#define ein_param_typestate(...) __attribute__((param_typestate(__VA_ARGS__)))
#else
#define ein_param_typestate(...)
#endif

/// \def ein_return_typestate(x)
/// The return_typestate attribute can be applied to functions or parameters. When applied to a function the attribute specifies the state of the returned value. The function’s body is checked to ensure that it always returns a value in the specified state. On the caller side, values returned by the annotated function are initialized to the given state.
/// When applied to a function parameter it modifies the state of an argument after a call to the function returns. The function’s body is checked to ensure that the parameter is in the expected state before returning.
#if ein_has_attribute(return_typestate)
#define ein_return_typestate(...) __attribute__((return_typestate(__VA_ARGS__)))
#else
#define ein_return_typestate(...)
#endif

/// \def ein_moving
/// short-hand for indicating that this argument was moved and is now consumed, assuming it was unconsumed before.
#define ein_moving ein_return_typestate(consumed) ein_param_typestate(unconsumed)

/// \def ein_set_typestate(x)
/// Annotate methods that transition an object into a new state with __attribute__((set_typestate(new_state))). The new state must be unconsumed, consumed, or unknown.
#if ein_has_attribute(set_typestate)
#define ein_set_typestate(...) __attribute__((set_typestate(__VA_ARGS__)))
#else
#define ein_set_typestate(...)
#endif

/// \def ein_test_typestate(x)
/// Use __attribute__((test_typestate(tested_state))) to indicate that
/// a method returns true if the object is in the specified state.
#if ein_has_attribute(test_typestate)
#  define ein_test_typestate(...) __attribute__((test_typestate(__VA_ARGS__)))
#else
#  define ein_test_typestate
#  define ein_test_typestate(...)
#endif // ein_has_attribute(test_typestate)

// thread safety analysis
// ======================
// these are supported by -Wthread-safety

/// \def ein_no_thread_safety_analysis
/// \brief Use __attribute__((no_thread_safety_analysis)) on a function declaration to specify that the thread safety analysis should not be run on that function. This attribute provides an escape hatch (e.g. for situations when it is difficult to annotate the locking policy).
#if ein_has_attribute(no_thread_safety_analysis)
#define ein_no_thread_safety_analysis __attribute__((no_thread_safety_analysis))
#else
#define ein_no_thread_safety_analysis
ein_message("no no_thread_safety_analysis")
#endif

/// \def ein_lockable
/// \brief Use __attribute__((lockable)) on a class definition to specify that it has a lockable type (e.g. a Mutex class). This annotation is primarily used to check consistency.
#if ein_has_attribute(lockable)
#define ein_lockable __attribute__((lockable))
#else
#define ein_lockable
ein_message("no lockable")
#endif

/// \def ein_scoped_lockable
/// \brief Use __attribute__((scoped_lockable)) on a class definition
/// to specify that it has a “scoped” lockable type. Objects of this
/// type will acquire the lock upon construction and release it upon
/// going out of scope. This annotation is primarily used to check
/// consistency.
#if ein_has_attribute(scoped_lockable)
#  define ein_scoped_lockable __attribute__((scoped_lockable))
#else
#  define ein_scoped_lockable
ein_message("no scoped_lockable")
#endif

/// \def ein_guarded_var
/// \brief Use __attribute__((guarded_var)) on a variable declaration
/// to specify that the variable must be accessed while holding some
/// lock.
#if ein_has_attribute(guarded_var)
#  define ein_guarded_var __attribute__((guarded_var))
#else
#  define ein_guarded_var
ein_message("no guarded_var")
#endif // ein_has_attribute(guarded_var)

/// \def ein_pt_guarded_var
/// \brief Use __attribute__((pt_guarded_var)) on a pointer
/// declaration to specify that the pointer must be dereferenced while
/// holding some lock.
#if ein_has_attribute(pt_guarded_var)
#  define ein_pt_guarded_var __attribute__((pt_guarded_var))
#else
#  define ein_pt_guarded_var
ein_message("no pt_guarded_var")
#endif // ein_has_attribute(pt_guarded_var)

/// \def ein_guarded_by(l)
/// \brief Use __attribute__((guarded_by(l))) on a variable
/// declaration to specify that the variable must be accessed while
/// holding lock l.
#if ein_has_attribute(guarded_by)
#  define ein_guarded_by(__x) __attribute__((guarded_by(__x)))
#else
#  define ein_guarded_by(__x)
ein_message("no guarded_by")
#endif // ein_has_attribute(guarded_by)

/// \def ein_pt_guarded_by(l)
/// \brief Use __attribute__((pt_guarded_by(l))) on a pointer declaration to specify that the pointer must be dereferenced while holding lock l.
#if ein_has_attribute(pt_guarded_by)
#define ein_pt_guarded_by(__x) __attribute__((pt_guarded_by(__x)))
#else
#define ein_pt_guarded_by(__x)
ein_message("no pt_guarded_by")
#endif

/// \def ein_acquired_before(...)
/// \brief Use __attribute__((acquired_before(...))) on a declaration
/// of a lockable variable to specify that the lock must be acquired
/// before all attribute arguments. Arguments must be lockable type,
/// and there must be at least one argument.
#if ein_has_attribute(acquired_before)
#  define ein_acquired_before(...) __attribute__((acquired_before(__VA_ARGS__)))
#else
#  define ein_acquired_before(...)
ein_message("no acquired_before")
#endif // ein_has_attribute(acquired_before)

/// \def ein_acquired_after(...)
/// \details
/// Use __attribute__((acquired_after(...))) on a declaration of a
/// lockable variable to specify that the lock must be acquired after
/// all attribute arguments. Arguments must be lockable type, and
/// there must be at least one argument.
#if ein_has_attribute(acquired_after)
#  define ein_acquired_after(...) __attribute__((acquired_after(__VA_ARGS__)))
#else
#  define ein_acquired_after(...)
ein_message("no acquired_after")
#endif // ein_has_attribute(acquired_after)

/// \def ein_exclusive_lock_function(...)
/// \details
/// Use __attribute__((exclusive_lock_function(...))) on a function
/// declaration to specify that the function acquires all listed locks
/// exclusively. This attribute takes zero or more arguments: either
/// of lockable type or integers indexing into function parameters of
/// lockable type. If no arguments are given, the acquired lock is
/// implicitly this of the enclosing object.
#if ein_has_attribute(exclusive_lock_function)
#  define ein_exclusive_lock_function(...) __attribute__((exclusive_lock_function(__VA_ARGS__)))
#else
#  define ein_exclusive_lock_function(...)
ein_message("no exclusive_lock_function")
#endif // ein_has_attribute(exclusive_lock_function)

/// \def ein_shared_trylock_function(...)
/// \details
/// Use __attribute__((shared_lock_function(...))) on a function
/// declaration to specify that the function will try (without
/// blocking) to acquire all listed locks, although the locks may be
/// shared (e.g. read locks). This attribute takes one or more
/// arguments. The first argument is an integer or boolean value
/// specifying the return value of a successful lock acquisition. The
/// remaining arugments are either of lockable type or integers
/// indexing into function parameters of lockable type. If only one
/// argument is given, the acquired lock is implicitly this of the
/// enclosing object.
#if ein_has_attribute(shared_trylock_function)
#  define ein_shared_trylock_function(...) __attribute__((shared_trylock_function(__VA_ARGS__)))
#else
#  define ein_shared_trylock_function(...)
ein_message("no shared_trylock_function")
#endif // ein_has_attribute(shared_trylock_function)

/// \def ein_unlock_function(...)
/// \details
/// Use __attribute__((unlock_function(...))) on a function declaration
/// to specify that the function release all listed locks. This
/// attribute takes zero or more arguments: either of lockable type or
/// integers indexing into function parameters of lockable type. If no
/// arguments are given, the acquired lock is implicitly this of the
/// enclosing object.
#if ein_has_attribute(unlock_function)
#  define ein_unlock_function(...) __attribute__((unlock_function(__VA_ARGS__)))
#else
#  define ein_unlock_function(...)
ein_message("no unlock_function")
#endif // ein_has_attribute(unlock_function)

/// \def ein_lock_returned(l)
/// \details
/// Use __attribute__((lock_returned(l))) on a function declaration to
/// specify that the function returns lock l (l must be of lockable
/// type). This annotation is used to aid in resolving lock
/// expressions.
#if ein_has_attribute(lock_returned)
#  define ein_lock_returned(__x) __attribute__((lock_returned(__x)))
#else
#  define ein_lock_returned(__x)
ein_message("no lock_returned")
#endif // ein_has_attribute(lock_returned)

/// \def ein_locks_excluded(...)
/// \details
/// Use __attribute__((locks_excluded(...))) on a function declaration
/// to specify that the function must not be called with the listed
/// locks. Arguments must be lockable type, and there must be at least
/// one argument.
#if ein_has_attribute(locks_excluded)
#  define ein_locks_excluded(...) __attribute__((locks_excluded(__VA_ARGS__)))
#else
#  define ein_locks_excluded(...)
ein_message("no locks_excluded")
#endif // ein_has_attribute(locks_excluded)

/// \def ein_exclusive_locks_required(...)
/// \details
/// Use __attribute__((exclusive_locks_required(...))) on a function
/// declaration to specify that the function must be called while
/// holding the listed exclusive locks. Arguments must be lockable
/// type, and there must be at least one argument.
#if ein_has_attribute(exclusive_locks_required)
#  define ein_exclusive_locks_required(...) __attribute__((exclusive_locks_required(__VA_ARGS__)))
#else
#  define ein_exclusive_locks_required(...)
ein_message("no exclusive_locks_required")
#endif // ein_has_attribute(exclusive_locks_required)

/// \def ein_shared_locks_required(...)
/// \details
/// Use __attribute__((shared_locks_required(...))) on a function
/// declaration to specify that the function must be called while
/// holding the listed shared locks. Arguments must be lockable type,
/// and there must be at least one argument.
#if ein_has_attribute(shared_locks_required)
#  define ein_shared_locks_required(...) __attribute__((shared_locks_required(__VA_ARGS__)))
#else
#  define ein_shared_locks_required(...)
ein_message("no shared_locks_required")
#endif // ein_has_attribute(shared_locks_required)

/// \def ein_preferred_name(x)
/// \brief portable `[[clang::preferred_name(x)]]` annotations
/// \details
/// can be applied to a template struct to provide better names
/// for some concrete instantiations
#if __has_cpp_attribute(clang::preferred_name)
#define ein_preferred_name(__x) [[clang::preferred_name(__x)]]
#else
#define ein_preferred_name(__x)
#endif

/// \def ein_acquire_handle(x)
/// \brief indicates this method returns an acquired resource
/// \details
/// names its type with a tag
/// if applied to a method it refers to the function result
/// if applied to an argument it indicates that argument is an out-parameter providing the value
#if __has_cpp_attribute(acquire_handle)
#define ein_acquire_handle(__x) [[clang::acquire_handle(__x)]]
#else
#define ein_acquire_handle(__x)
#endif

/// \def ein_release_handle(x)
/// \brief indicates this argument is an acquired resource that is being released, with a tag
#if __has_cpp_attribute(release_handle)
#define ein_release_handle(__x) [[clang::release_handle(__x)]]
#else
#define ein_release_handle(__x)
#endif

/// \def ein_use_handle(x)
/// \brief indicates this argument is a use of a resource, but does not release it.
#if __has_cpp_attribute(use_handle)
#define ein_use_handle(__x) [[clang::use_handle(__x)]]
#else
#define ein_use_handle(__x)
#endif

/// \def ein_flatten
/// \brief portable `[[flatten]]`
/// \details
/// the compiler should inline recursively aggressively under
/// this definition. use with care, good for expression templates
#if ein_has_attribute(flatten)
#define ein_flatten __attribute__((flatten))
#else
#define ein_flatten
ein_message("no flatten")
#endif

/// \def ein_malloc
/// \brief portable `__attribute__((malloc))` attribute
/// \details
/// indicates the returned memory does not alias with any other pointer

#if ein_has_attribute(malloc)
#define ein_malloc __attribute__((malloc))
#else
#define ein_malloc
ein_message("no malloc")
#endif

/// \def ein_uninitialized
/// \brief portable `[[clang::uninitialized]]` attribute
/// \details
/// ensures a stack variable remains uninitialized regardless of
/// `-ftrivial-auto-var-init=*` settings passed to the compiler

#if ein_has_attribute(uninitialized)
#define ein_uninitialized __attribute__((uninitialized))
#else
#define ein_uninitialized
ein_message("no unitialized")
#endif

/// \def ein_noalias
/// \brief portable `__declspec(noalias)`
/// \details
/// indicates a function only accesses data through pointer arguments
///
/// requires clang to be running with `-fdeclspec` or `-fms-extensions`
/// but can aid alias analysis
#if ein_has_declspec_attribute(noalias)
#define ein_noalias __declspec(noalias)
#else
#define ein_noalias
ein_message("no noalias")
#endif

/// \def ein_constinit
/// \brief portable `constinit`
/// \details
/// `constinit` requires C++20, but __attribute__((require_constant_initialization)) is available earlier
#if ein_has_attribute(require_constant_initialization)
#define ein_constinit __attribute__((require_constant_initialization))
#else
#define ein_constinit
ein_message("no constinit")
#endif

//#if ein_has_attribute(called_once)
//#define EIN_CALLED_ONCE __attribute__((called_once))
//#else
//#define EIN_CALLED_ONCE
//ein_message(no called_once)
//#endif

/// \def ein_const
/// \brief portable `__attribute__((const))`. NOT `const`
/// \details
/// indicates the result is entirely determined by the arguments
/// and does not access main memory in any way, including accessing
/// members of this
///
/// allows the compiler to easily elide/duplicate calls
#if ein_has_attribute(const)
#define ein_const __attribute__((const))
#else
#define ein_const
ein_message("no const")
#endif

/// \def ein_pure
/// \brief portable `__attribute__((pure))`
/// \details
/// no side-effects other than return value, may inspect globals
///
/// allows the compiler to easily elide/duplicate calls
#if ein_has_attribute(pure)
#define ein_pure __attribute__((pure))
#else
#define ein_pure
ein_message("no pure")
#endif

/// \def ein_reinitializes
/// \brief portable `[[clang::reinitializes]]`
/// \details
/// indicates to any uninitialised object state sanitizer
/// that this restores an object to a fresh state independent
/// of its previous state
#if __has_cpp_attribute(clang::reinitializes)
#define ein_reinitializes [[clang::reinitializes]]
#else
#define ein_reinitializes
ein_message("no reinitializes")
#endif

/// \def ein_assume_aligned(x)
/// \brief portable `__attribute__((assume_aligned(x)))`
/// \details
/// function returns a pointer with alignment that is at least x

#if ein_has_attribute(assume_aligned)
#define ein_assume_aligned(x) __attribute__((assume_aligned(x)))
#else
#define ein_assume_aligned(x)
ein_message("no assume_aligned")
#endif

/// \def ein_align_value(x)
/// \brief portable `__attribute__((align_value(x)))`
/// \details
/// the annotated pointer specified has alignment at least x

#if ein_has_attribute(align_value)
#define ein_align_value(x) __attribute__((align_value(x)))
#else
#define ein_align_value(x)
ein_message("no align_value")
#endif

/// \def ein_null_terminated_string_arg(x)
/// \brief portable `__attribute__((align_value(x)))`
/// \details
/// the annotated pointer specified has alignment at least x

#if ein_has_attribute(null_terminated_string_arg)
#define ein_null_terminated_string_arg(x) __attribute__((null_terminated_string_arg(x)))
#else
#define ein_null_terminated_string_arg(x)
// #warning no null_terminated_string_arg
#endif

/// \def ein_alloc_align(x)
/// \brief portable `__attribute__((alloc_align(x)))`
/// \details
/// Indicates the 1-based argument number of a function that indicates the alignment of the returned result
#if ein_has_attribute(alloc_align)
#define ein_alloc_align(x) __attribute__((alloc_align(x)))
#else
#define ein_alloc_align(x)
ein_message("no alloc_align")
#endif

/// \def ein_alloc_size(x)
/// \brief portable `__attribute__((alloc_size(x)))`
/// \details
/// arg # (1-based) of the attribute that tells you the size of the result in bytes
#if ein_has_attribute(alloc_size)
#define ein_alloc_size(x) __attribute__((alloc_size(x)))
#else
#define ein_alloc_size(x)
ein_message("no alloc_size")
#endif

/// \def ein_noescape
/// \brief portable `__attribute__((noescape))`
/// \details
/// argument is not captured by the function (rust-style borrow)

#if ein_has_attribute(noescape)
#define ein_noescape __attribute__((noescape))
#else
#define ein_noescape
ein_message("no noescape")
#endif

/// \def ein_callback(...)
/// \brief portable `__attribute__((callback(...)))`
/// \details
/// indicates the specified argument will be called back with the
/// other named arguments. complicated, see clang docs.
///
/// allows better interprocedural analysis

#if ein_has_attribute(callback)
#define ein_callback(...) __attribute__((callback(__VA_ARGS__)))
#else
#define ein_callback(...)
ein_message("no callback")
#endif

/// \def ein_lifetimebound
/// \brief portable `__attribute__((lifetimebound))`
/// \details
/// the argument must be kept alive as long as the result of the
/// function is alive. Should be used for functions that return
/// references or views into the target object.
///
/// Sadly, clang (as of Jan 2021) does not yet allow it to be
/// applied to the implicit this argument of an member
/// function

#if ein_has_attribute(lifetimebound)
#define ein_lifetimebound [[clang::lifetimebound]]
#else
#define ein_lifetimebound
ein_message("no lifetimebound")
#endif

/// \def ein_returns_nonnull
/// \brief portable `__attribute__((returns_nonnnull))`
/// \details
/// The result of this function will be non-null

#if ein_has_attribute(returns_nonnull)
#define ein_returns_nonnull __attribute__((returns_nonnull))
#else
#define ein_returns_nonnull
ein_message("no returns_nonnull")
#endif

/// \def ein_nonnull(...)
/// \brief portable `__attribute__((nonnnull(...)))`
/// \details
/// Indicates the selected (1-based) indexed arguments to this function
/// must be non-null. Passing a null pointer to such an argument is
/// undefined behavior. GCC-style.

#if ein_has_attribute(nonnull)
#define ein_nonnull(...) __attribute__((nonnull(__VA_ARGS__)))
#else
#define ein_nonnull(...)
ein_message("no gcc-style nonnull")
#endif

/// \def ein_noreturn
/// \brief portable `__attribute__((noreturn)))`
/// \details
/// Indicates the method does not return to the caller.

#if ein_has_attribute(noreturn)
#define ein_noreturn __attribute__((noreturn))
#else
#define ein_noreturn
ein_message("no gcc-style noreturn")
#endif

/// \def ein_Nonnull
/// \brief portable `_Nonnull`
/// \details
/// can be applied to each * in an data type to indicate that argument should never be null.
///
/// \def ein_Nullable
/// \brief portable `_Nullable`
/// \details
/// can be applied to each * in an data type to indicate that argument might be null.
///
/// \def ein_Null_unspecified
/// \brief portable `_Null_unspecified`
/// \details
/// applied to each * in an data type to indicate that the nullability of it is unknown or complicated
#ifdef __clang__
#define ein_Nonnull _Nonnull
#define ein_Nullable _Nullable
#define ein_Null_unspecified _Null_unspecified
#else
#define ein_Nonnull
#define ein_Nullable
#define ein_Null_unspecified
#endif


/// \def ein_host
/// \brief portable `__host__` for cuda
/// \details
/// indicates the function should be available on the host
///
/// \def ein_device
/// \brief portable `__device__` for cuda
/// \details
/// indicates the function should be available on the device
///
/// \def ein_global
/// \brief portable `__global__` for cuda
/// \details
/// indicates the function is a global entry point for a
/// compute kernel

#ifdef __CUDACC__
#define ein_host __host__
#define ein_device __device__
#define ein_global __global__
#else
#define ein_host
#define ein_device
#define ein_global
#endif

/// \def ein_hd
/// \brief applies both \ref ein_host and \ref ein_device
/// \details
/// most functions in the library should be defined this way
///
/// may eventually add similar annotations for sycl and the like

#define ein_hd ein_host ein_device

/// \def ein_no_unique_address
/// \brief portable c++20 `[[no_unique_address]]`
/// \details
/// clang makes this available slightly earlier

#if __has_cpp_attribute(no_unique_address)
#define ein_no_unique_address [[no_unique_address]]
#else
#define ein_no_unique_address
#endif

#if 0

/// \def ein_empty_bases
/// \brief portable __declspec(empty_bases)
/// \details
/// just for msvc for now.

#if defined(_MSC_VER) && !defined(__clang__)
#define ein_empty_bases __declspec(empty_bases)
#else
#define ein_empty_bases
#endif

#endif

/// \def ein_maybe_unused
/// \brief argument, member or definition `[[maybe_unused]]`
/// \since C++17
#define ein_maybe_unused [[maybe_unused]]

/// \def ein_deprecated
/// \brief `[[deprecated]]`
/// \since C++14
#define ein_deprecated [[deprecated]]

/// \def ein_nodiscard
/// \brief C++17 `[[nodiscard]]`.
/// \details
/// The user should explicitly throw away the result rather than let it be silently discarded
#define ein_nodiscard [[nodiscard]]

/// \def ein_unroll
/// \brief hint to unroll the following loop N times. (clang-specific)
/// \def ein_unroll_full
/// \brief hint to unroll the following loop fully. (clang-specific)
#ifdef __clang__
#define ein_unroll(N) ein_pragma(clang loop unroll_count(N))
#define ein_unroll_full ein_pragma(clang loop unroll(full))
#else
#define ein_unroll(N)
#define ein_unroll_full
#endif

/// \}
