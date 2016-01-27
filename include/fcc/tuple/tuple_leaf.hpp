//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_TUPLE_TUPLE_LEAF_HPP
#define FCC_TUPLE_TUPLE_LEAF_HPP

#include <fcc/tuple/tuple_element_access.hpp>

#include <fcc/detail/generic_traits.hpp>

namespace fcc { namespace detail {
    template<typename T, typename... Us>
    concept bool TupleLeafConstructible = std::is_constructible<T, Us&&...>::value;
    
    template<typename T, typename Alloc, typename... Us>
    concept bool TupleLeafNoAllocConstructible =
        Allocator<Alloc> &&
        std::is_constructible<T, Us&&...>::value &&
        std::is_same<uses_alloc_ctor<T, Alloc, Us...>, no_alloc_ctor>::value;
    
    template<typename T, typename Alloc, typename... Us>
    concept bool TupleLeafLastAllocConstructible =
        Allocator<Alloc> &&
        std::is_constructible<T, Us&&..., const Alloc&>::value &&
        std::is_same<uses_alloc_ctor<T, Alloc, Us...>, alloc_last_ctor>::value;
    
    template<typename T, typename Alloc, typename... Us>
    concept bool TupleLeafAllocArgConstructible =
        Allocator<Alloc> &&
        std::is_constructible<T, allocator_arg_t, const Alloc&, Us&&...>::value &&
        std::is_same<uses_alloc_ctor<T, Alloc, Us...>, alloc_arg_ctor>::value;
    
    template<typename T, typename UTuple, std::size_t... Is>
    concept bool TupleLeafPiecewiseConstructible =
        std::is_constructible<T, get_type<Is, UTuple&&>...>::value;
    
    template<typename Leaf, typename Alloc, typename UTuple, std::size_t... Is>
    concept bool TupleLeafPiecewiseAllocConstructible =
        Allocator<Alloc> &&
        std::is_constructible<Leaf, allocator_arg_t, const Alloc&, get_type<Is, UTuple&&>...>{}();

    template<typename T>
    using empty_not_final = meta::bool_<std::is_empty<T>{} && !std::is_final<T>{}>;
    
    template<typename T>
    concept bool EBOCandidate = std::is_empty<T>::value && !std::is_final<T>::value;
    
    // unique type (no EBO)
    template<std::size_t I, typename T>
    struct tuple_leaf
    {
    private:
        T value;
        
    public:
        // no defaults, and don't participate in overload resolution
        tuple_leaf(const tuple_leaf& rhs)            requires false = delete;
        tuple_leaf(tuple_leaf&& rhs)                 requires false = delete;
        tuple_leaf& operator=(const tuple_leaf& rhs) requires false = delete;
        tuple_leaf& operator=(tuple_leaf&& rhs)      requires false = delete;
        
        template<typename... Us>
            requires TupleLeafConstructible<T, Us...>
        constexpr tuple_leaf(Us&&... us)
            noexcept(std::is_nothrow_constructible<T, Us&&...>::value)
            : value(std::forward<Us>(us)...)
        {}
        
        template<typename Alloc, typename... Us>
            requires TupleLeafNoAllocConstructible<T, Alloc, Us...>
        constexpr tuple_leaf(allocator_arg_t, const Alloc&, Us&&... us)
            noexcept(std::is_nothrow_constructible<T, Us&&...>::value)
            : value(std::forward<Us>(us)...)
        {}
        
        template<typename Alloc, typename... Us>
            requires TupleLeafLastAllocConstructible<T, Alloc, Us...>
        constexpr tuple_leaf(allocator_arg_t, const Alloc& alloc, Us&&... us)
            noexcept(std::is_nothrow_constructible<T, Us&&..., const Alloc&>::value)
            : value(std::forward<Us>(us)..., alloc)
        {}
        
        template<typename Alloc, typename... Us>
            requires TupleLeafAllocArgConstructible<T, Alloc, Us...>
        constexpr tuple_leaf(allocator_arg_t, const Alloc& alloc, Us&&... us)
            noexcept(std::is_nothrow_constructible<T, allocator_arg_t, const Alloc&, Us&&...>{}())
            : value(allocator_arg, alloc, std::forward<Us>(us)...)
        {}
        
        template<std::size_t... Is, TupleLike UTuple>
            requires TupleLeafPiecewiseConstructible<T, UTuple, Is...>
        constexpr tuple_leaf(piecewise_construct_t, meta::iseq<Is...>, UTuple&& utuple)
            noexcept(std::is_nothrow_constructible<T, get_type<Is, UTuple&&>...>::value)
            : value(get_I<Is>(std::forward<UTuple>(utuple))...)
        {}
        
        template<std::size_t... Is, typename Alloc, TupleLike UTuple>
            requires TupleLeafPiecewiseAllocConstructible<tuple_leaf, Alloc, UTuple, Is...>
        constexpr tuple_leaf(piecewise_construct_t, meta::iseq<Is...>, allocator_arg_t,
                             const Alloc& alloc, UTuple&& utuple)
            noexcept(std::is_nothrow_constructible<tuple_leaf, allocator_arg_t, const Alloc&,
                                                   get_type<Is, UTuple&&>...>::value)
            : tuple_leaf(allocator_arg, alloc, get<Is>(std::forward<UTuple>(utuple))...)
        {}
        
        constexpr T& get() noexcept
        { return static_cast<T&>(value); }
        
        constexpr const T& get() const noexcept
        { return static_cast<const T&>(value); }
        
        constexpr volatile T& get() volatile noexcept
        { return static_cast<volatile T&>(value); }
        
        constexpr const volatile T& get() const volatile noexcept
        { return static_cast<const volatile T&>(value); }
    };
    
    // unique type (EBO)
    template<std::size_t I, EBOCandidate T>
    struct tuple_leaf<I, T> : private T
    {
    public:
        // no defaults, and don't participate in overload resolution
        tuple_leaf(const tuple_leaf& rhs)            requires false = delete;
        tuple_leaf(tuple_leaf&& rhs)                 requires false = delete;
        tuple_leaf& operator=(const tuple_leaf& rhs) requires false = delete;
        tuple_leaf& operator=(tuple_leaf&& rhs)      requires false = delete;
        
        template<typename... Us>
            requires TupleLeafConstructible<T, Us...>
        constexpr tuple_leaf(Us&&... us)
            noexcept(std::is_nothrow_constructible<T, Us&&...>::value)
            : T(std::forward<Us>(us)...)
        {}
        
        template<typename Alloc, typename... Us>
            requires TupleLeafNoAllocConstructible<T, Alloc, Us...>
        constexpr tuple_leaf(allocator_arg_t, const Alloc&, Us&&... us)
            noexcept(std::is_nothrow_constructible<T, Us&&...>::value)
            : T(std::forward<Us>(us)...)
        {}
        
        template<typename Alloc, typename... Us>
            requires TupleLeafLastAllocConstructible<T, Alloc, Us...>
        constexpr tuple_leaf(allocator_arg_t, const Alloc& alloc, Us&&... us)
            noexcept(std::is_nothrow_constructible<T, Us&&..., const Alloc&>::value)
            : T(std::forward<Us>(us)..., alloc)
        {}
        
        template<typename Alloc, typename... Us>
            requires TupleLeafAllocArgConstructible<T, Alloc, Us...>
        constexpr tuple_leaf(allocator_arg_t, const Alloc& alloc, Us&&... us)
            noexcept(std::is_nothrow_constructible<T, allocator_arg_t, const Alloc&, Us&&...>{}())
            : T(allocator_arg, alloc, std::forward<Us>(us)...)
        {}
        
        template<std::size_t... Is, TupleLike UTuple>
            requires TupleLeafPiecewiseConstructible<T, UTuple, Is...>
        constexpr tuple_leaf(piecewise_construct_t, meta::iseq<Is...>, UTuple&& utuple)
            noexcept(std::is_nothrow_constructible<T, get_type<Is, UTuple&&>...>::value)
            : T(get_I<Is>(std::forward<UTuple>(utuple))...)
        {}
        
        template<typename Alloc, std::size_t... Is, TupleLike UTuple>
            requires TupleLeafPiecewiseAllocConstructible<tuple_leaf, Alloc, UTuple, Is...>
        constexpr tuple_leaf(piecewise_construct_t, allocator_arg_t, const Alloc& alloc,
                             meta::iseq<Is...>, UTuple&& utuple)
            noexcept(std::is_nothrow_constructible<tuple_leaf, allocator_arg_t, const Alloc&,
                                                   get_type<Is, UTuple&&>...>::value)
            : tuple_leaf(allocator_arg, alloc, get<Is>(std::forward<UTuple>(utuple))...)
        {}
        
        constexpr T& get() noexcept
        { return static_cast<T&>(*this); }
        
        constexpr const T& get() const noexcept
        { return static_cast<const T&>(*this); }
        
        constexpr volatile T& get() volatile noexcept
        { return static_cast<volatile T&>(*this); }
        
        constexpr const volatile T& get() const volatile noexcept
        { return static_cast<const volatile T&>(*this); }
    };
}}

#endif
