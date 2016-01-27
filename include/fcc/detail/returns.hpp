//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_DETAIL_RETURNS_HPP
#define FCC_DETAIL_RETURNS_HPP

// macros lifted or adapted from range-v3
#define FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(...) \
    noexcept(noexcept(decltype(__VA_ARGS__)(__VA_ARGS__)))\
    -> decltype(__VA_ARGS__) \
    { return (__VA_ARGS__) ; }

#define FCC_RETURN_NOEXCEPT(...) \
    noexcept(noexcept(decltype(__VA_ARGS__)(__VA_ARGS__)))\
    { return (__VA_ARGS__) ; }

#endif
