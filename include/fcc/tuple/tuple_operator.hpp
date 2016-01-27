//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_TUPLE_TUPLE_OPERATORS_HPP
#define FCC_TUPLE_TUPLE_OPERATORS_HPP

#include <fcc/tuple/tuple_traits.hpp>

namespace fcc {
    /////////////////////////////////////////
    // operator==(TupleLike&&, TupleLike&&)
    /////////////////////////////////////////
    template<typename Tup0, typename Tup1>
    using is_tuple_tuple_equality_comparable =
        detail::tuple_get_based_trait_template<meta::quote<is_equality_comparable>, Tup0, Tup1>;
    
    template<typename Tup0, typename Tup1>
    concept bool TupleTupleEqualityComparable = is_tuple_tuple_equality_comparable<Tup0, Tup1>{}();

    //////////////////////////////////////////////////
    // operator==(TupleLike&&, TupleLike&&) noexcept
    //////////////////////////////////////////////////
    template<typename Tup0, typename Tup1>
    using is_nothrow_tuple_tuple_equality_comparable =
        detail::tuple_get_based_trait_template<meta::quote<is_nothrow_equality_comparable>, Tup0,
                                               Tup1>;
    
    ////////////////////////////////////////
    // operator<(TupleLike&&, TupleLike&&)
    ////////////////////////////////////////
    template<typename Tup0, typename Tup1>
    using is_tuple_tuple_less_than_comparable =
        detail::commutative_get_based_trait<meta::quote<is_less_than_comparable>, Tup0, Tup1>;
    
    template<typename Tup0, typename Tup1>
    concept bool TupleTupleLessThanComparable = is_tuple_tuple_less_than_comparable<Tup0, Tup1>{}();
    
    /////////////////////////////////////////////////
    // operator<(TupleLike&&, TupleLike&&) noexcept
    /////////////////////////////////////////////////
    template<typename Tup0, typename Tup1>
    using is_nothrow_tuple_tuple_less_than_comparable =
        detail::commutative_get_based_trait<meta::quote<is_nothrow_less_than_comparable>, Tup0,
                                            Tup1>;
    
    namespace detail {
        template<TupleLike Tup0, TupleLike Tup1, std::size_t... Is>
        constexpr bool equal_impl(Tup0&& tup0, Tup1&& tup1, meta::iseq<Is...>)
            noexcept(is_nothrow_tuple_tuple_equality_comparable<Tup0, Tup1>::value)
        {
            return (bool(get<Is>(std::forward<Tup0>(tup0)) == get<Is>(std::forward<Tup1>(tup1))) &&
                    ...);
        }
    }

    inline namespace operators {
        ///////////////
        // operator==
        ///////////////
        template<fcc::TupleLike Tup0, fcc::TupleLike Tup1>
            requires fcc::TupleTupleEqualityComparable<Tup0, Tup1>
        constexpr bool operator==(Tup0&& tup0, Tup1&& tup1)
            noexcept(fcc::is_nothrow_tuple_tuple_equality_comparable<Tup0, Tup1>::value)
        {
            return fcc::detail::equal_impl(std::forward<Tup0>(tup0), std::forward<Tup1>(tup1),
                                           fcc::tuple_indices<Tup0>{});
        }

        ///////////////
        // operator!=
        ///////////////
        template<fcc::TupleLike Tup0, fcc::TupleLike Tup1>
            requires fcc::TupleTupleEqualityComparable<Tup0, Tup1>
        constexpr bool operator!=(Tup0&& tup0, Tup1&& tup1)
            noexcept(fcc::is_nothrow_tuple_tuple_equality_comparable<Tup0, Tup1>::value)
        { return !(std::forward<Tup0>(tup0) == std::forward<Tup1>(tup1)); }
    } // inline namespace operators
    
    // inspiration from libc++ and tao::tuple
    namespace detail {
        template<TupleLike Tup0, TupleLike Tup1, std::size_t... Is>
        constexpr bool less_impl(Tup0&& tup0, Tup1&& tup1, meta::iseq<Is...>)
            noexcept(is_nothrow_tuple_tuple_less_than_comparable<Tup0, Tup1>::value)
        {
            bool result = false;
            (void)(... || ((result = static_cast<bool>(get<Is>(std::forward<Tup0>(tup0)) <
                                                       get<Is>(std::forward<Tup1>(tup1)))) ||
                           static_cast<bool>(get<Is>(std::forward<Tup1>(tup1)) <
                                             get<Is>(std::forward<Tup0>(tup0)))));
            return result;
        }
    }

    inline namespace operators {
        //////////////
        // operator<
        //////////////
        template<fcc::TupleLike Tup0, fcc::TupleLike Tup1>
            requires fcc::TupleTupleLessThanComparable<Tup0, Tup1>
        constexpr bool operator<(Tup0&& tup0, Tup1&& tup1)
            noexcept(fcc::is_nothrow_tuple_tuple_less_than_comparable<Tup0, Tup1>::value)
        {
            return fcc::detail::less_impl(std::forward<Tup0>(tup0), std::forward<Tup1>(tup1),
                                          fcc::tuple_indices<Tup0>{});
        }

        //////////////
        // operator>
        //////////////
        template<fcc::TupleLike Tup0, fcc::TupleLike Tup1>
            requires fcc::TupleTupleLessThanComparable<Tup1, Tup0>
        constexpr bool operator>(Tup0&& tup0, Tup1&& tup1)
            noexcept(fcc::is_nothrow_tuple_tuple_less_than_comparable<Tup1, Tup0>::value)
        { return std::forward<Tup1>(tup1) < std::forward<Tup0>(tup0); }

        ///////////////
        // operator<=
        ///////////////
        template<fcc::TupleLike Tup0, fcc::TupleLike Tup1>
            requires fcc::TupleTupleLessThanComparable<Tup1, Tup0>
        constexpr bool operator<=(Tup0&& tup0, Tup1&& tup1)
            noexcept(fcc::is_nothrow_tuple_tuple_less_than_comparable<Tup1, Tup0>::value)
        { return !(std::forward<Tup1>(tup1) < std::forward<Tup0>(tup0)); }

        ///////////////
        // operator>=
        ///////////////
        template<fcc::TupleLike Tup0, fcc::TupleLike Tup1>
            requires fcc::TupleTupleLessThanComparable<Tup0, Tup1>
        constexpr bool operator>=(Tup0&& tup0, Tup1&& tup1)
            noexcept(fcc::is_nothrow_tuple_tuple_less_than_comparable<Tup0, Tup1>::value)
        { return !(std::forward<Tup0>(tup0) < std::forward<Tup1>(tup1)); }
    }
}

#endif
