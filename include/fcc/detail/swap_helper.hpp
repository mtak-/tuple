//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_DETAIL_SWAP_HELPER_HPP
#define FCC_DETAIL_SWAP_HELPER_HPP

#include <fcc/detail/returns.hpp>

#include <type_traits>
#include <utility>

namespace fcc
{
    namespace detail_swap_adl
    {
        using std::swap;
        
        template<typename T, typename U>
        constexpr auto swap_(T&& t, U&& u, long)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            swap(std::forward<T>(t), std::forward<U>(u))
        )
        
        template<typename T, typename U>
        constexpr auto swap_(T&& t, U&& u, int)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            std::forward<T>(t).swap(std::forward<U>(u))
        )
    }
    
    template<typename T, typename U>
    constexpr auto forward_swap(T&& t, U&& u)
    FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
        detail_swap_adl::swap_(std::forward<T>(t), std::forward<U>(u), 42)
    )
}

#endif
