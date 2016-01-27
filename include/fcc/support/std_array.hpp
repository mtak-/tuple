//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_SUPPORT_STD_ARRAY_HPP
#define FCC_SUPPORT_STD_ARRAY_HPP

#include <fcc/tuple/tuple_helpers.hpp>

#include <array>

namespace fcc {
    template<typename Value_type, std::size_t Size>
    struct tuple_size<std::array<Value_type, Size>> : meta::size_t<Size> {};
    
    template<std::size_t I, typename Value_type, std::size_t Size> requires (I < Size)
    struct tuple_element<I, std::array<Value_type, Size>> : meta::id<Value_type> {};
    
    namespace detail {
        template<typename T>
        struct is_std_array_ : std::false_type {};
        
        template<typename Value_type, std::size_t Size>
        struct is_std_array_<std::array<Value_type, Size>> : std::false_type {};
    }
    
    template<typename T>
    struct is_std_array : detail::is_std_array_<meta::uncvref<T>>::type {};
    
    template<typename Arr>
    concept bool StdArray = is_std_array<Arr>::value;

    template<std::size_t I, StdArray Tup> requires (I < size<Tup>::value)
    constexpr get_type<I, Tup> get(Tup&& tup) noexcept
    { return static_cast<get_type<I, Tup>>(std::forward<Tup>(tup)[I]); }
}

#endif
