#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \ingroup attributes */


#include "common.hpp"

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
