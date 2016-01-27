//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_DETAIL_STATIC_CONST_HPP
#define FCC_DETAIL_STATIC_CONST_HPP

// same as range-v3
namespace fcc
{
    template<typename T> struct static_const { static constexpr T value {}; };
    template<typename T> constexpr T static_const<T>::value;
}

#endif
