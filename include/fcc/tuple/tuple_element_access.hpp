//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_TUPLE_TUPLE_ELEMENT_ACCESS_HPP
#define FCC_TUPLE_TUPLE_ELEMENT_ACCESS_HPP

#include <fcc/support/c_array.hpp>
#include <fcc/support/empty.hpp>

#include <fcc/detail/static_const.hpp>

namespace fcc {    
    // imported
    using std::get;
    
    //////////////
    // TUPLELIKE
    //////////////
    namespace detail {
        // was getting ICE's (I'm a concept noob) and this particular way works
        // (actually it doesn't, had to remove the noexcept check for get<I>(tup),
        // all the noexcept traits below depend on get<I>(tup) being noexcept, so obviously
        // this is not desirable)
        template<typename T, std::size_t I>
        concept bool TupleLikeI = requires(T t) {
            {std::is_same<decltype(get<I>(std::forward<T>(t))), get_type<I, T>>{}} -> std::true_type;
//            {meta::bool_<noexcept(get<I>(std::forward<T>(t)))>{}} -> std::true_type;
        };
        
        template<typename T, std::size_t... Is>
        concept bool TupleLikeImpl = (TupleLikeI<T, Is> && ...);
        
        template<typename T, typename ISeq>
        struct is_tuple_like_ : std::false_type {};
        
        template<typename T, std::size_t... Is> requires TupleLikeImpl<T, Is...>
        struct is_tuple_like_<T, meta::iseq<Is...>> : std::true_type {};
        
        template<typename T>
        concept bool TupleSizeable = requires {
            {std::is_same<meta::uncvref<decltype(tuple_size<meta::uncvref<T>>::value)>,
                          std::size_t>{}} -> std::true_type;
        };
    }
    
    template<typename T>
    struct is_tuple_like : std::false_type {};
    
    template<detail::TupleSizeable T>
    struct is_tuple_like<T> : detail::is_tuple_like_<T, indices<T>>::type {};
    
    template<typename T>
    concept bool TupleLike = is_tuple_like<T>::value;
    
    /////////////////////////
    // UNQUALIFIEDTUPLELIKE
    /////////////////////////
    // probably only useful to constrain constructors
    template<typename T>
    concept bool UnqualifiedTupleLike = TupleLike<T> && std::is_same<meta::uncvref<T>, T>::value;
    
    // element access by type (can be defined in terms of get<I>, tuple_element_t and
    // tuple_size - the requirements for TupleLike)
    namespace detail {
        template<TupleLike Tup, template<std::size_t, typename> class Transform, typename ISeq>
        struct tuple_as_list_;
        
        template<TupleLike Tup, template<std::size_t, typename> class Transform, std::size_t... Is>
        struct tuple_as_list_<Tup, Transform, meta::iseq<Is...>>
            : meta::id<meta::list<Transform<Is, Tup>...>> {};
        
        template<typename Tup, template<std::size_t, typename> class Transform>
        using tuple_as_list = meta::_t<tuple_as_list_<Tup, Transform, indices<Tup>>>;
        
        template<TupleLike Tup>
        using tuple_elem_as_list = meta::_t<tuple_as_list_<Tup, elem, indices<Tup>>>;
        
        template<TupleLike Tup>
        using tuple_get_type_as_list = meta::_t<tuple_as_list_<Tup, get_type, indices<Tup>>>;
        
        struct apply_tuple
        {
            template<typename... Ts>
            using apply = tuple<Ts...>;
        };
        
        template<typename T, TupleLike Tup, typename = indices<Tup>>
        struct unique_in_tuple;
        
        template<typename T, TupleLike Tup, std::size_t... Is>
        struct unique_in_tuple<T, Tup, meta::iseq<Is...>>
        {
            using find = meta::find_index<tuple_elem_as_list<Tup>, T>;
            using rfind = meta::reverse_find_index<tuple_elem_as_list<Tup>, T>;
            static const bool value = std::is_same<find, rfind>::value &&
                                      !std::is_same<meta::npos, find>::value;
            static const int index = find::value;
        };
    }
    
    //////////
    // GET_I
    //////////
    namespace detail {
        template<std::size_t I>
        struct get_I_fn {
            template<TupleLike Tup> requires (I < size<Tup>::value)
            constexpr get_type<I, Tup> operator()(Tup&& tup) const noexcept
            { return get<I>(std::forward<Tup>(tup)); }
        };
    }
    namespace {
        template<std::size_t I>
        constexpr auto&& get_I = static_const<detail::get_I_fn<I>>::value;
    }
    
    //////////
    // GET_T
    //////////
    namespace detail {
        template<typename T>
        struct get_T_fn {
            template<TupleLike Tup> requires detail::unique_in_tuple<T, Tup>::value
            constexpr get_type<detail::unique_in_tuple<T, Tup>::index, Tup>
            operator()(Tup&& tup) const noexcept
            { return get<detail::unique_in_tuple<T, Tup>::index>(std::forward<Tup>(tup)); }
        };
    }
    namespace {
        template<typename T>
        constexpr auto&& get_T = static_const<detail::get_T_fn<T>>::value;
    }
    
    ///////////
    // GET<T>
    ///////////
    template<typename T, TupleLike Tup> requires detail::unique_in_tuple<T, Tup>::value
    constexpr get_type<detail::unique_in_tuple<T, Tup>::index, Tup> get(Tup&& tup) noexcept
    { return get_T<T>(std::forward<Tup>(tup)); }
}

#endif
