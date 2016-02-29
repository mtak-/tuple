//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_TUPLE_TUPLE_CREATION_HPP
#define FCC_TUPLE_TUPLE_CREATION_HPP

#include <fcc/tuple/tuple_traits.hpp>

namespace fcc
{
    namespace detail {
        struct ignore_t
        {
            explicit ignore_t() = default;
            
            template<typename T>
            void operator=(T&&) const volatile {}
        };
    }
    static_assert(std::is_trivial<detail::ignore_t>{}, "");
    
    ///////////
    // IGNORE
    ///////////
    namespace { constexpr auto ignore = detail::ignore_t(); }
    
    namespace detail {
        template<typename T> struct unwrap_ : meta::id<T> {};
        template<typename T> struct unwrap_<std::reference_wrapper<T>> : meta::id<T&> {};
        template<typename T> using unwrap = meta::_t<unwrap_<T>>;
        template<typename T> using make_tuple_elem_type = unwrap<std::decay_t<T>>;
    }
    
    ///////////////
    // MAKE_TUPLE
    ///////////////
    struct make_tuple_fn
    {
        template<typename... Ts>
        constexpr tuple<detail::make_tuple_elem_type<Ts>...> operator()(Ts&&... ts) const
        FCC_RETURN_NOEXCEPT(
            tuple<detail::make_tuple_elem_type<Ts>...>{std::forward<Ts>(ts)...}
        )
    };
    namespace { constexpr auto&& make_tuple = static_const<make_tuple_fn>::value; }
    
    /////////////////////
    // FORWARD_AS_TUPLE
    /////////////////////
    struct forward_as_tuple_fn {
        template<typename... Ts>
        constexpr tuple<Ts&&...> operator()(Ts&&... ts) const noexcept
        { return tuple<Ts&&...>{std::forward<Ts>(ts)...}; }
    };
    namespace { constexpr auto&& forward_as_tuple = static_const<forward_as_tuple_fn>::value; }
    
    ////////
    // TIE
    ////////
    struct tie_fn {
        template<typename... Ts>
        constexpr tuple<Ts&...> operator()(Ts&... ts) const noexcept
        { return tuple<Ts&...>{ts...}; }
    };
    namespace { constexpr auto&& tie = static_const<tie_fn>::value; }
    
    // lots of inspiration (and copy paste) from Eric Niebler's blog post
    // http://ericniebler.com/2014/11/13/tiny-metaprogramming-library/
    namespace detail {        
        template<TupleLike... Tups>
        struct tuple_cat_return_type_
            : meta::id<meta::apply_list<apply_tuple, meta::concat<tuple_elem_as_list<Tups>...>>> {};
        
        template<typename... Tups>
        using tuple_cat_return_type = meta::_t<tuple_cat_return_type_<Tups...>>;
        
        template<typename... Tups>
        using tuple_cat_constructible =
            meta::apply_list<
                meta::quote<std::is_constructible>,
                meta::concat<
                    meta::list<tuple_cat_return_type<Tups...>>,
                    tuple_get_type_as_list<Tups>...>>;
        
        template<typename... Tups>
        using tuple_cat_inner =
            meta::join<
                meta::transform<
                    meta::list<tuple_elem_as_list<Tups>...>,
                    meta::transform<
                        meta::iseq_as_list<meta::make_iseq<sizeof...(Tups)>>,
                        meta::quote<meta::always>>,
                    meta::quote<meta::transform>>>;
        
        template<typename... Tups>
        using tuple_cat_outer =
            meta::join<
                meta::transform<
                    meta::list<tuple_elem_as_list<Tups>...>,
                    meta::compose<
                        meta::quote<meta::iseq_as_list>,
                        meta::quote_i<std::size_t, meta::make_iseq>,
                        meta::quote<meta::list_size>>>>;
    }
    
    //////////////
    // TUPLE_CAT
    //////////////
    struct tuple_cat_fn {
    private:
        // Note: at the latest, as of Feb 28, 2016, ICE affecting literal rvalue refs passed in
        // here no longer happens!!!
        // old repro: auto z = fcc::tuple_cat(fcc::tuple<int&&>{8});
        //            cout << (get<0>(z)) << endl;
        template<typename ResultTuple, std::size_t... Outers, std::size_t... Inners,
                 typename TupOfTups>
        static constexpr ResultTuple
        impl(meta::list<meta::size_t<Outers>...>, meta::list<meta::size_t<Inners>...>,
             TupOfTups&& tupOfTups)
        FCC_RETURN_NOEXCEPT(
            ResultTuple{get<Outers>(get<Inners>(std::forward<TupOfTups>(tupOfTups)))...}
        )
    
    public:
        template<typename... Tups>
            requires (TupleLike<Tups> && ... && detail::tuple_cat_constructible<Tups...>::value)
        constexpr detail::tuple_cat_return_type<Tups...> operator()(Tups&&... tuples) const
        FCC_RETURN_NOEXCEPT(
            impl<detail::tuple_cat_return_type<Tups...>>(
                detail::tuple_cat_outer<Tups...>{},
                detail::tuple_cat_inner<Tups...>{},
                fcc::forward_as_tuple(std::forward<Tups>(tuples)...))
        )
    };
    namespace { constexpr auto&& tuple_cat = static_const<tuple_cat_fn>::value; }
}

#endif
