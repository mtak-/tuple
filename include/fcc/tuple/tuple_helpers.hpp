//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_TUPLE_TUPLE_HELPERS_HPP
#define FCC_TUPLE_TUPLE_HELPERS_HPP

#include <meta/meta.hpp>

namespace fcc {
    template<typename... Ts>
    struct tuple;
    
    ///////////////
    // TUPLE_SIZE
    ///////////////
    template<typename T>
    struct tuple_size;
    
    template<typename... Ts>
    struct tuple_size<tuple<Ts...>> : meta::size_t<sizeof...(Ts)> {};
    
    template<typename... Ts>
    struct tuple_size<std::tuple<Ts...>> : meta::size_t<sizeof...(Ts)> {};
    
    template<typename T, typename U>
    struct tuple_size<std::pair<T, U>> : meta::size_t<2> {};
    
    template<typename T>
    struct tuple_size<const T> : tuple_size<T>::type {};
    
    template<typename T>
    struct tuple_size<volatile T> : tuple_size<T>::type {};
    
    template<typename T>
    struct tuple_size<const volatile T> : tuple_size<T>::type {};
    
    namespace {
        template<class T>
        constexpr std::size_t tuple_size_v = tuple_size<T>::value;
    }
    
    /////////
    // SIZE
    /////////
    template<typename Tup>
    using size = tuple_size<std::remove_reference_t<Tup>>;
    
    //////////////////
    // TUPLE_INDICES
    //////////////////
    template<typename Tup>
    using tuple_indices = meta::make_iseq<size<Tup>::value>;
    
    //////////////////
    // TUPLE_ELEMENT
    //////////////////
    template<std::size_t I, typename T>
    struct tuple_element;
    
    template<std::size_t I, typename... Ts> requires (I < sizeof...(Ts))
    struct tuple_element<I, tuple<Ts...>> : meta::id<meta::at_c<meta::list<Ts...>, I>> {};
    
    template<std::size_t I, typename... Ts> requires (I < sizeof...(Ts))
    struct tuple_element<I, std::tuple<Ts...>> : meta::id<meta::at_c<meta::list<Ts...>, I>> {};
    
    template<typename T, typename U>
    struct tuple_element<0, std::pair<T, U>> : meta::id<T> {};
    
    template<typename T, typename U>
    struct tuple_element<1, std::pair<T, U>> : meta::id<U> {};
    
    template<std::size_t I, typename T> requires (I < tuple_size<T>::value)
    struct tuple_element<I, const T> : meta::id<meta::_t<tuple_element<I, T>>> {};
    
    template<std::size_t I, typename T> requires (I < tuple_size<T>::value)
    struct tuple_element<I, volatile T> : meta::id<meta::_t<tuple_element<I, T>>> {};
    
    template<std::size_t I, typename T> requires (I < tuple_size<T>::value)
    struct tuple_element<I, const volatile T> : meta::id<meta::_t<tuple_element<I, T>>> {};
    
    template<std::size_t I, typename T>
    using tuple_element_t = meta::_t<tuple_element<I, T>>;
    
    /////////////
    // GET_TYPE
    /////////////
    template<std::size_t I, typename T>
    using get_type = meta::as_same_cvref<tuple_element_t<I, std::remove_reference_t<T>>, T>;
}

#endif
