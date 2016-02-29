//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_TUPLE_TUPLE_IMPL_HPP
#define FCC_TUPLE_TUPLE_IMPL_HPP

#include <fcc/tuple/tuple_leaf.hpp>
#include <fcc/tuple/tuple_traits.hpp>

namespace fcc { namespace detail {
    // used by derived class to select a constructor (easier/faster)
    struct initialize_t { explicit initialize_t() = default; };
    namespace { constexpr auto&& initialize = static_const<initialize_t>::value; }
    
    struct tuple_initialize_t { explicit tuple_initialize_t() = default; };
    namespace { constexpr auto&& tuple_initialize = static_const<tuple_initialize_t>::value; }
    
    template<typename ISeq, typename... Ts>
    struct tuple_base;
    
    template<std::size_t... Is, typename... Ts>
    struct tuple_base<meta::iseq<Is...>, Ts...> : tuple_leaf<Is, Ts>...
    {
    private:
        template<std::size_t I>
        using base = tuple_leaf<I, meta::at_c<meta::list<Ts...>, I>>;
        using derived = tuple<Ts...>;
    
    public:
        // no defaults, and don't participate in overload resolution
        tuple_base(const tuple_base& rhs)            requires false = delete;
        tuple_base(tuple_base&& rhs)                 requires false = delete;
        tuple_base& operator=(const tuple_base& rhs) requires false = delete;
        tuple_base& operator=(tuple_base&& rhs)      requires false = delete;
        
        template<std::size_t... Js, std::size_t... Ds, typename... Us>
        constexpr tuple_base(initialize_t, meta::iseq<Js...>, meta::iseq<Ds...>, Us&&... us)
            noexcept(is_nothrow_tuple_constructible<derived, Us&&...>::value)
            : base<Js>(std::forward<Us>(us))...
            , base<Ds>()...
        {}
        
        template<typename Alloc, std::size_t... Js, std::size_t... Ds, typename... Us>
        constexpr tuple_base(allocator_arg_t, const Alloc& alloc, meta::iseq<Js...>,
                             meta::iseq<Ds...>, Us&&... us)
            noexcept(is_nothrow_tuple_alloc_constructible<derived, const Alloc&, Us&&...>::value)
            : base<Js>(allocator_arg, alloc, std::forward<Us>(us))...
            , base<Ds>(allocator_arg, alloc)...
        {}
        
        template<typename UTuple>
        constexpr tuple_base(tuple_initialize_t, UTuple&& rhs)
            noexcept(is_nothrow_tuple_tuple_constructible<derived, UTuple>::value)
            : base<Is>(get<Is>(std::forward<UTuple>(rhs)))...
        {}
        
        template<typename Alloc, typename UTuple>
        constexpr tuple_base(tuple_initialize_t, allocator_arg_t, const Alloc& alloc, UTuple&& rhs)
            noexcept(is_nothrow_tuple_tuple_alloc_constructible<derived, const Alloc&, UTuple>{}())
            : base<Is>(allocator_arg, alloc, get<Is>(std::forward<UTuple>(rhs)))...
        {}
        
        template<std::size_t... Js, std::size_t... Ds, typename... UTuples>
        constexpr tuple_base(piecewise_construct_t, meta::iseq<Js...>, meta::iseq<Ds...>,
                             UTuples&&... utuples)
            noexcept(is_nothrow_piecewise_tuple_constructible<derived, UTuples...>::value)
            : base<Js>(piecewise_construct, indices<UTuples>{}, std::forward<UTuples>(utuples))...
            , base<Ds>()...
        {}
        
        template<typename Alloc, std::size_t... Js, std::size_t... Ds, typename... UTuples>
        constexpr tuple_base(piecewise_construct_t, allocator_arg_t, const Alloc& alloc,
                             meta::iseq<Js...>, meta::iseq<Ds...>, UTuples&&... utuples)
            noexcept(is_nothrow_piecewise_tuple_alloc_constructible<derived, const Alloc&,
                                                                    UTuples...>::value)
            : base<Js>(piecewise_construct, allocator_arg, alloc, indices<UTuples>{},
                       std::forward<UTuples>(utuples))...
            , base<Ds>(allocator_arg, alloc)...
        {}
    };
}}

#endif
