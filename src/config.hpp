//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_SRC_CONFIG_HPP
#define FCC_SRC_CONFIG_HPP

// the type to test as a tuple element
// some sample types are in test_types.hpp
#ifndef TYPE
#define TYPE int
#endif

// whether or not to use std::tuple instead for outputting
// constructibility/assignability (0/1)
#ifndef USE_STD_TUPLE
#define USE_STD_TUPLE 0
#endif

#if USE_STD_TUPLE == 1
#define TUPLE_TYPE std::tuple
#else
#define TUPLE_TYPE fcc::tuple
#endif

#ifdef NO_CONSTRUCTION
#undef NO_CONSTRUCTION
#define NO_CONSTRUCTION(...) /**/
#else
#define NO_CONSTRUCTION(...) __VA_ARGS__
#endif

#ifdef NO_ASSIGNMENT
#define NO_LVAL_ASSIGNMENT
#define NO_RVAL_ASSIGNMENT
#endif

#ifdef NO_LVAL_ASSIGNMENT
#undef NO_LVAL_ASSIGNMENT
#define NO_LVAL_ASSIGNMENT(...) /**/
#else
#define NO_LVAL_ASSIGNMENT(...) __VA_ARGS__
#endif

#ifdef NO_RVAL_ASSIGNMENT
#undef NO_RVAL_ASSIGNMENT
#define NO_RVAL_ASSIGNMENT(...) /**/
#else
#define NO_RVAL_ASSIGNMENT(...) __VA_ARGS__
#endif

#endif
