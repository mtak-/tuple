//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_SUPPORT_EMPTY_HPP
#define FCC_SUPPORT_EMPTY_HPP

#include <fcc/tuple/tuple_helpers.hpp>

#include <fcc/detail/generic_traits.hpp>

namespace fcc {
    // TODO: fails from time to time
//    template<typename T>
//        requires is_complete<T>::value && std::is_empty<T>::value && std::is_pod<T>::value
//    struct tuple_size<T> : meta::size_t<0> {};
}

#endif
