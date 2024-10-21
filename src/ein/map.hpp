#pragma once

/// \file
/// \brief variadic macro utilities
/// \author William Swanson
/// \author Edward Kmett

/// \defgroup macros macros
/// \brief variadic macro utilities
/// \ingroup internals_group

#define EIN_EVAL0_(...) __VA_ARGS__
#define EIN_EVAL1_(...) EIN_EVAL0_(EIN_EVAL0_(EIN_EVAL0_(__VA_ARGS__)))
#define EIN_EVAL2_(...) EIN_EVAL1_(EIN_EVAL1_(EIN_EVAL1_(__VA_ARGS__)))
#define EIN_EVAL3_(...) EIN_EVAL2_(EIN_EVAL2_(EIN_EVAL2_(__VA_ARGS__)))
#define EIN_EVAL4_(...) EIN_EVAL3_(EIN_EVAL3_(EIN_EVAL3_(__VA_ARGS__)))
#define EIN_EVAL_(...)  EIN_EVAL4_(EIN_EVAL4_(EIN_EVAL4_(__VA_ARGS__)))

#define EIN_MAP_END_(...)
#define EIN_MAP_OUT_
#define EIN_MAP_COMMA_ ,

#define EIN_MAP_GET_END2_() 0, EIN_MAP_END_
#define EIN_MAP_GET_END1_(...) EIN_MAP_GET_END2_
#define EIN_MAP_GET_END_(...) EIN_MAP_GET_END1_
#define EIN_MAP_NEXT0_(test, next, ...) next EIN_MAP_OUT_
#define EIN_MAP_NEXT1_(test, next) EIN_MAP_NEXT0_(test, next, 0)
#define EIN_MAP_NEXT_(test, next)  EIN_MAP_NEXT1_(EIN_MAP_GET_END_ test, next)

#define EIN_MAP0_(f, x, peek, ...) f(x) EIN_MAP_NEXT_(peek, EIN_MAP1_)(f, peek, __VA_ARGS__)
#define EIN_MAP1_(f, x, peek, ...) f(x) EIN_MAP_NEXT_(peek, EIN_MAP0_)(f, peek, __VA_ARGS__)

#define EIN_MAP_LIST_NEXT1_(test, next) EIN_MAP_NEXT0_(test, EIN_MAP_COMMA_ next, 0)
#define EIN_MAP_LIST_NEXT_(test, next)  EIN_MAP_LIST_NEXT1_(EIN_MAP_GET_END_ test, next)
#define EIN_MAP_LIST0_(f, x, peek, ...) f(x) EIN_MAP_LIST_NEXT_(peek, EIN_MAP_LIST1_)(f, peek, __VA_ARGS__)
#define EIN_MAP_LIST1_(f, x, peek, ...) f(x) EIN_MAP_LIST_NEXT_(peek, EIN_MAP_LIST0_)(f, peek, __VA_ARGS__)

#define EIN_JOIN_(X,Y) X ## Y
#define ein_(Y) EIN_JOIN_(ein_,Y)

/// \{

/// Applies macro `f` to each parameter.
#define EIN_MAP(f, ...) EIN_EVAL_(EIN_MAP1_(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

/// Applies macro `f` to each parameter. Inserts commas between the results.
#define EIN_MAP_LIST(f, ...) EIN_EVAL_(EIN_MAP_LIST1_(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

/// \}

/*
 * Copyright (C) 2012 William Swanson, 2021 Edward Kmett
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the names of the authors or
 * their institutions shall not be used in advertising or otherwise to
 * promote the sale, use or other dealings in this Software without
 * prior written authorization from the authors.
 */
