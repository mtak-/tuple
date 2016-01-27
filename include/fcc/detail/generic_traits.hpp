//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_DETAIL_GENERIC_TRAITS_HPP
#define FCC_DETAIL_GENERIC_TRAITS_HPP

#include <fcc/detail/allocator_concept.hpp>
#include <fcc/detail/swap_helper.hpp>

#include <meta/meta.hpp>

#include <iosfwd>
#include <type_traits>

namespace fcc {
    ///////////////////
    // is_convertible
    ///////////////////
    namespace detail {
        template<typename To> void convertible_test(To) noexcept;

        template<typename To, typename List, typename = void>
        struct convertible_multi_param : std::false_type {};

        template<typename To, typename... Froms>
        struct convertible_multi_param<To, meta::list<Froms...>,
            meta::void_<decltype(convertible_test<To>({std::declval<Froms>()...}))>>
            : std::is_constructible<To, Froms...>::type
        {};

        template<typename To, typename From>
        using convertible_single_param = meta::_t<std::is_convertible<From, To>>;
    }
    
    template<typename To, typename... Froms>
    struct is_convertible;
    
    template<typename To, typename From>
    struct is_convertible<To, From> : detail::convertible_single_param<To, From>::type {};
    
    // this fails to give an accurate result when a type has std::initializer_list
    // constructors that overlap with other constructors (std::vector).
    // there's no way to work around it afaik
    template<typename To, typename... Froms> requires (sizeof...(Froms) != 1)
    struct is_convertible<To, Froms...>
        : detail::convertible_multi_param<To, meta::list<Froms...>>::type {};

    template<typename To, typename From>
    concept bool Convertible = is_convertible<From, To>::value;
    
    ////////////////
    // is_complete
    ////////////////
    namespace detail {
        template<typename T, typename = void>
        struct is_complete_ : std::false_type {};
        
        template<typename T>
        struct is_complete_<T, meta::void_<decltype(sizeof(T))>> : std::true_type {};
    }
    
    template<typename T>
    using is_complete = meta::_t<detail::is_complete_<T>>;
    
    template<typename T>
    concept bool Complete = is_complete<T>::value;
    
    ////////////////////////
    // piecewise_construct
    ////////////////////////
    struct piecewise_construct_t { explicit piecewise_construct_t() = default; };
    static_assert(std::is_trivial<piecewise_construct_t>{}, "");
    namespace { constexpr auto piecewise_construct = piecewise_construct_t(); }
    
    //////////////////
    // allocator_arg
    //////////////////
    struct allocator_arg_t { explicit allocator_arg_t() = default; };
    static_assert(std::is_trivial<allocator_arg_t>{}, "");
    namespace { constexpr auto allocator_arg = allocator_arg_t(); }
    
    ///////////////////
    // uses_allocator
    ///////////////////
    template<typename T, typename Alloc>
    struct uses_allocator : std::false_type
    { static_assert(Allocator<Alloc>, "Type passed to uses_allocator is not an allocator!"); };
    
    template<typename T, Allocator Alloc>
    struct uses_allocator<T, Alloc> : std::uses_allocator<T, Alloc>::type {};
    
    template<typename T, Allocator Alloc>
    struct uses_allocator<const T, Alloc> : uses_allocator<T, Alloc>::type {};
    
    template<typename T, Allocator Alloc>
    struct uses_allocator<volatile T, Alloc> : uses_allocator<T, Alloc>::type {};
    
    template<typename T, Allocator Alloc>
    struct uses_allocator<const volatile T, Alloc> : uses_allocator<T, Alloc>::type {};
    
    struct no_alloc_ctor {};
    struct alloc_last_ctor {};
    struct alloc_arg_ctor {};
    struct alloc_failed_ctor {}; // allocator passed to constructor is the wrong type
    
    template<typename T, typename Alloc, typename... Args>
    using uses_alloc_ctor =
            std::conditional_t<
                uses_allocator<T, std::remove_reference_t<Alloc>>::value,
                std::conditional_t<
                    std::is_constructible<T, allocator_arg_t, Alloc, Args...>::value,
                    alloc_arg_ctor,
                    std::conditional_t<
                        std::is_constructible<T, Args..., Alloc>::value,
                        alloc_last_ctor,
                        alloc_failed_ctor>>,
                no_alloc_ctor>;
    
    ////////////////////////////////
    // is_uses_alloc_constructible
    ////////////////////////////////
    namespace detail {
        template<typename Uses, typename T, typename Alloc, typename... Args>
        struct is_uses_alloc_constructible_ : std::true_type {};
        
        template<typename T, typename Alloc, typename... Args>
        struct is_uses_alloc_constructible_<no_alloc_ctor, T, Alloc, Args...>
            : std::is_constructible<T, Args...> {};
        
        template<typename T, typename Alloc, typename... Args>
        struct is_uses_alloc_constructible_<alloc_failed_ctor, T, Alloc, Args...> : std::false_type
        {
            static_assert(!std::is_same<T, T>{},
                          "allocator passed to constructor does not match the expected type");
        };
    }
    
    template<typename T, typename Alloc, typename... Args>
    struct is_uses_alloc_constructible : std::false_type {};
    
    template<typename T, Allocator Alloc, typename... Args>
    struct is_uses_alloc_constructible<T, Alloc, Args...>
        : detail::is_uses_alloc_constructible_<uses_alloc_ctor<T, Alloc, Args...>, T, Alloc,
                                               Args...>::type
    {};
    
    ////////////////////////////////////////
    // is_nothrow_uses_alloc_constructible
    ////////////////////////////////////////
    namespace detail {
        template<typename Uses, typename T, typename Alloc, typename... Args>
        struct is_nothrow_uses_alloc_constructible_;
        
        template<typename T, typename Alloc, typename... Args>
        struct is_nothrow_uses_alloc_constructible_<no_alloc_ctor, T, Alloc, Args...>
            : std::is_nothrow_constructible<T, Args...> {};
        
        template<typename T, typename Alloc, typename... Args>
        struct is_nothrow_uses_alloc_constructible_<alloc_last_ctor, T, Alloc, Args...>
            : std::is_nothrow_constructible<T, Args..., Alloc> {};
        
        template<typename T, typename Alloc, typename... Args>
        struct is_nothrow_uses_alloc_constructible_<alloc_arg_ctor, T, Alloc, Args...>
            : std::is_nothrow_constructible<allocator_arg_t, Alloc, T, Args...> {};
        
        template<typename T, typename Alloc, typename... Args>
        struct is_nothrow_uses_alloc_constructible_<alloc_failed_ctor, T, Alloc, Args...>
            : std::false_type
        {
            static_assert(!std::is_same<T, T>{},
                          "allocator passed to constructor does not match the expected type");
        };
    }
    
    template<typename T, typename Alloc, typename... Args>
    struct is_nothrow_uses_alloc_constructible : std::false_type {};
    
    template<typename T, Allocator Alloc, typename... Args>
    struct is_nothrow_uses_alloc_constructible<T, Alloc, Args...>
        : detail::is_nothrow_uses_alloc_constructible_<uses_alloc_ctor<T, Alloc, Args...>, T, Alloc,
                                                       Args...>::type
    {};
    
    //////////////////////////////
    // is_uses_alloc_convertible
    //////////////////////////////
    namespace detail {
        template<typename Uses, typename T, typename Alloc, typename... Args>
        struct is_uses_alloc_convertible_;
        
        template<typename T, typename Alloc, typename... Args>
        struct is_uses_alloc_convertible_<no_alloc_ctor, T, Alloc, Args...>
            : is_convertible<T, Args...> {};
        
        template<typename T, typename Alloc, typename... Args>
        struct is_uses_alloc_convertible_<alloc_last_ctor, T, Alloc, Args...>
            : is_convertible<T, Args..., Alloc> {};
        
        template<typename T, typename Alloc, typename... Args>
        struct is_uses_alloc_convertible_<alloc_arg_ctor, T, Alloc, Args...>
            : is_convertible<allocator_arg_t, Alloc, T, Args...> {};
        
        template<typename T, typename Alloc, typename... Args>
        struct is_uses_alloc_convertible_<alloc_failed_ctor, T, Alloc, Args...> : std::false_type
        {
            static_assert(!std::is_same<T, T>{},
                          "allocator passed to constructor does not match the expected type");
        };
    }
    
    template<typename T, typename Alloc, typename... Args>
    struct is_uses_alloc_convertible : std::false_type {};
    
    template<typename T, Allocator Alloc, typename... Args>
    struct is_uses_alloc_convertible<T, Alloc, Args...>
        : detail::is_uses_alloc_convertible_<uses_alloc_ctor<T, Alloc, Args...>, T, Alloc,
                                             Args...>::type
    {};
    
    ///////////////////////////////////
    // bound_uses_alloc_constructible
    ///////////////////////////////////
    template<typename Alloc>
    struct bound_uses_alloc_constructible
    {
        template<typename T, typename... Args>
        using apply = is_uses_alloc_constructible<T, Alloc, Args...>;
    };
    
    ///////////////////////////////////////////
    // bound_nothrow_uses_alloc_constructible
    ///////////////////////////////////////////
    template<typename Alloc>
    struct bound_nothrow_uses_alloc_constructible
    {
        template<typename T, typename... Args>
        using apply = is_nothrow_uses_alloc_constructible<T, Alloc, Args...>;
    };
    
    /////////////////////////////////
    // bound_uses_alloc_convertible
    /////////////////////////////////
    template<typename Alloc>
    struct bound_uses_alloc_convertible
    {
        template<typename T, typename... Args>
        using apply = is_uses_alloc_convertible<T, Alloc, Args...>;
    };
    
    ///////////////////
    // swap(T&&, U&&)
    ///////////////////
    template<typename T, typename U = T>
    concept bool Swappable = requires(T t, U u)
    { detail_swap_adl::swap_(std::forward<T>(t), std::forward<U>(u), 42); };
    
    template<typename T, typename U = T>
    struct is_swappable : meta::bool_<Swappable<T, U>> {};
    
    ////////////////////////////
    // swap(T&&, U&&) noexcept
    ////////////////////////////
    template<typename T, typename U = T>
    concept bool NothrowSwappable = Swappable<T, U> && requires(T t, U u)
    { {detail_swap_adl::swap_(std::forward<T>(t), std::forward<U>(u), 42)} noexcept; };
    
    template<typename T, typename U = T>
    struct is_nothrow_swappable : meta::bool_<NothrowSwappable<T, U>> {};
    
    /////////////////////////
    // operator==(T&&, U&&)
    /////////////////////////
    template<typename T, typename U = T>
    concept bool EqualityComparable = requires(T t, U u)
    { {std::forward<T>(t) == std::forward<U>(u)} -> bool; };
    
    template<typename T, typename U = T>
    struct is_equality_comparable : meta::bool_<EqualityComparable<T, U>> {};
    
    //////////////////////////////////
    // operator==(T&&, U&&) noexcept
    //////////////////////////////////
    template<typename T, typename U = T>
    concept bool NothrowEqualityComparable = EqualityComparable<T, U> && requires(T t, U u)
    { {std::forward<T>(t) == std::forward<U>(u)} noexcept -> bool; };
    
    template<typename T, typename U = T>
    struct is_nothrow_equality_comparable : meta::bool_<NothrowEqualityComparable<T, U>> {};
    
    ////////////////////////
    // operator<(T&&, U&&)
    ////////////////////////
    template<typename T, typename U = T>
    concept bool LessThanComparable = requires(T t, U u)
    { {std::forward<T>(t) < std::forward<U>(u)} -> bool; };
    
    template<typename T, typename U = T>
    struct is_less_than_comparable : meta::bool_<LessThanComparable<T, U>> {};
    
    /////////////////////////////////
    // operator<(T&&, U&&) noexcept
    /////////////////////////////////
    template<typename T, typename U = T>
    concept bool NothrowLessThanComparable = LessThanComparable<T, U> && requires(T t, U u)
    { {std::forward<T>(t) < std::forward<U>(u)} noexcept -> bool; };
    
    template<typename T, typename U = T>
    struct is_nothrow_less_than_comparable : meta::bool_<NothrowLessThanComparable<T, U>> {};
    
    ////////////////////////////////
    // is_base_of<std::istream, U>
    ////////////////////////////////
    template<typename T>
    struct is_istream : std::is_base_of<std::istream, meta::uncvref<T>>::type {};
    
    template<typename T>
    concept bool Istream = is_istream<T>::value;
    
    ////////////////////////////////
    // is_base_of<std::ostream, U>
    ////////////////////////////////
    template<typename T>
    struct is_ostream : std::is_base_of<std::ostream, meta::uncvref<T>>::type {};
    
    template<typename T>
    concept bool Ostream = is_ostream<T>::value;
    
    /////////////////////////
    // operator>>(T&&, U&&)
    /////////////////////////
    template<typename T, typename U>
    concept bool Istreamable = Istream<T> && requires(T t, U u)
    { std::forward<T>(t) >> std::forward<U>(u); };
    
    template<typename T, typename U>
    struct is_istreamable : meta::bool_<Istreamable<T, U>> {};
    
    //////////////////////////////////
    // operator>>(T&&, U&&) noexcept
    //////////////////////////////////
    template<typename T, typename U>
    concept bool NothrowIstreamable = Istreamable<T, U> && requires(T t, U u)
    { {std::forward<T>(t) >> std::forward<U>(u)} noexcept; };
    
    template<typename T, typename U>
    struct is_nothrow_istreamable : meta::bool_<NothrowIstreamable<T, U>> {};
    
    /////////////////////////
    // operator<<(T&&, U&&)
    /////////////////////////
    template<typename T, typename U>
    concept bool Ostreamable = Ostream<T> && requires(T t, U u)
    { std::forward<T>(t) << std::forward<U>(u); };
    
    template<typename T, typename U>
    struct is_ostreamable : meta::bool_<Ostreamable<T, U>> {};
    
    //////////////////////////////////
    // operator<<(T&&, U&&) noexcept
    //////////////////////////////////
    template<typename T, typename U>
    concept bool NothrowOstreamable = Ostreamable<T, U> && requires(T t, U u)
    { {std::forward<T>(t) << std::forward<U>(u)} noexcept; };
    
    template<typename T, typename U>
    struct is_nothrow_ostreamable : meta::bool_<NothrowOstreamable<T, U>> {};
    
    /////////////////////
    // braced_init_type
    /////////////////////
    template<typename T>
    using braced_init_type = std::remove_cv_t<T>;
}

#endif
