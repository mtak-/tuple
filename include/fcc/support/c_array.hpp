//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_SUPPORT_C_ARRAY_HPP
#define FCC_SUPPORT_C_ARRAY_HPP

#include <fcc/tuple/tuple_helpers.hpp>

namespace fcc {
    template<typename Value_type, std::size_t Size>
    struct tuple_size<Value_type[Size]> : meta::size_t<Size> {};
    
    template<typename Value_type, std::size_t Size>
    struct tuple_size<const Value_type[Size]> : meta::size_t<Size> {};
    
    template<typename Value_type, std::size_t Size>
    struct tuple_size<volatile Value_type[Size]> : meta::size_t<Size> {};
    
    template<typename Value_type, std::size_t Size>
    struct tuple_size<const volatile Value_type[Size]> : meta::size_t<Size> {};
    
    template<std::size_t I, typename Value_type, std::size_t Size> requires (I < Size)
    struct tuple_element<I, Value_type[Size]> : meta::id<Value_type> {};
    
    template<std::size_t I, typename Value_type, std::size_t Size> requires (I < Size)
    struct tuple_element<I, const Value_type[Size]> : meta::id<const Value_type> {};
    
    template<std::size_t I, typename Value_type, std::size_t Size> requires (I < Size)
    struct tuple_element<I, volatile Value_type[Size]> : meta::id<volatile Value_type> {};
    
    template<std::size_t I, typename Value_type, std::size_t Size> requires (I < Size)
    struct tuple_element<I, const volatile Value_type[Size]> : meta::id<const volatile Value_type> {};
    
    template<typename Arr>
    concept bool C_array = std::is_array<meta::uncvref<Arr>>::value;

    template<std::size_t I, C_array Tup> requires (I < size<Tup>::value)
    constexpr get_type<I, Tup> get(Tup&& tup) noexcept
    { return static_cast<get_type<I, Tup>>(std::forward<Tup>(tup)[I]); }
}

#endif
