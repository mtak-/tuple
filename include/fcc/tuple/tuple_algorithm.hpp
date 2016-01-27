//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_TUPLE_TUPLE_ALGORITHMS_HPP
#define FCC_TUPLE_TUPLE_ALGORITHMS_HPP

#include <fcc/tuple/tuple_creation.hpp>

#include <fcc/invoke.hpp>

namespace fcc {
    namespace detail {
        template<typename T, std::size_t J, std::size_t N> requires J != N - 1
        static constexpr T& forward_last(std::remove_reference_t<T>& t) noexcept
        { return static_cast<T&>(t); }
        
        template<typename T, std::size_t J, std::size_t N> requires J == N - 1
        static constexpr T&& forward_last(std::remove_reference_t<T>& t) noexcept
        { return static_cast<T&&>(t); }
    }

    /////////////////
    // TUPLE_ASSIGN
    /////////////////
    struct assign_fn {
    private:
        template<TupleLike Tup0, TupleLike Tup1, std::size_t... Is>
        static constexpr Tup0&& impl(Tup0&& lhs, Tup1&& rhs, meta::iseq<Is...>)
            noexcept(is_nothrow_tuple_tuple_assignable<Tup0&&, Tup1&&>::value)
        {
            (void(get<Is>(std::forward<Tup0>(lhs)) = get<Is>(std::forward<Tup1>(rhs))) , ...);
            return std::forward<Tup0>(lhs);
        }
    
    public:
        template<TupleLike Tup0, TupleLike Tup1>
            requires TupleTupleAssignable<Tup0&&, Tup1&&>
        constexpr Tup0&& operator()(Tup0&& lhs, Tup1&& rhs) const
            noexcept(is_nothrow_tuple_tuple_assignable<Tup0&&, Tup1&&>::value)
        { return impl(std::forward<Tup0>(lhs), std::forward<Tup1>(rhs), tuple_indices<Tup0>{}); }
    };
    namespace { constexpr auto&& assign = static_const<assign_fn>::value; }
    
    //////////////////////
    // swap(TupleLike&&)
    //////////////////////
    template<typename Tup0, typename Tup1>
    struct is_tuple_tuple_swappable
        : detail::tuple_get_based_trait_template<meta::quote<is_swappable>, Tup0, Tup1> {};
    
    template<typename Tup0, typename Tup1>
    concept bool TupleTupleSwappable = is_tuple_tuple_swappable<Tup0, Tup1>::value;
    
    ///////////////////////////////
    // swap(TupleLike&&) noexcept
    ///////////////////////////////
    template<typename Tup0, typename Tup1>
    struct is_nothrow_tuple_tuple_swappable
        : detail::tuple_get_based_trait_template<meta::quote<is_nothrow_swappable>, Tup0, Tup1> {};
    
    /////////
    // SWAP
    /////////
    struct swap_fn {
    private:
        template<TupleLike T, TupleLike U, std::size_t... Is>
        static constexpr void impl(T&& t, U&& u, meta::iseq<Is...>)
            noexcept(is_nothrow_tuple_tuple_swappable<T, U>::value)
        { (void(forward_swap(get<Is>(std::forward<T>(t)), get<Is>(std::forward<U>(u)))) , ...); }
    
    public:
        template<TupleLike T, TupleLike U>
            requires TupleTupleSwappable<T&&, U&&>
        constexpr void operator()(T&& t, U&& u) const
            noexcept(is_nothrow_tuple_tuple_swappable<T, U>::value)
        { impl(std::forward<T>(t), std::forward<U>(u), tuple_indices<T>{}); }
    };
    namespace { constexpr auto&& swap = static_const<swap_fn>::value; }
    
    /////////
    // HEAD
    /////////
    namespace { constexpr auto&& head = get_I<0>; }
    
    /////////
    // TAIL
    /////////
    struct tail_fn {
    private:
        template<TupleLike Tup, std::size_t I, std::size_t... Is>
        static constexpr auto impl(Tup&& tup, meta::iseq<I, Is...>)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            tuple<tuple_element_t<Is, std::remove_reference_t<Tup>>...>
                (get<Is>(std::forward<Tup>(tup))...)
        )
    
    public:
        template<TupleLike Tup>
            requires !NullTuple<Tup>
        constexpr auto operator()(Tup&& tup) const
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            impl(std::forward<Tup>(tup), tuple_indices<Tup>{})
        )
    };
    namespace { constexpr auto&& tail = static_const<tail_fn>::value; }
    
    // TODO: simplify traits after this point
    namespace detail {
        template<typename F, typename Tup, typename ISeq = tuple_indices<Tup>>
        struct is_tuple_applyable_;
        
        template<typename F, typename Tup, std::size_t... Is>
        struct is_tuple_applyable_<F, Tup, meta::iseq<Is...>>
            : is_invokable<F&&, get_type<Is, Tup>...>::type {};
    }
    
    template<typename F, typename Tup>
    struct is_tuple_applyable : std::false_type {};
    
    template<typename F, TupleLike Tup>
    struct is_tuple_applyable<F, Tup> : detail::is_tuple_applyable_<F, Tup>::type {};
    
    template<typename F, typename Tup>
    concept bool TupleApplyable() { return is_tuple_applyable<F, Tup>::value; }
    
    namespace detail {
        template<typename F, typename Tup, typename ISeq = tuple_indices<Tup>>
        struct is_nothrow_tuple_applyable_impl;
        
        template<typename F, typename Tup, std::size_t... Is>
        struct is_nothrow_tuple_applyable_impl<F, Tup, meta::iseq<Is...>>
            : is_nothrow_invokable<F&&, get_type<Is, Tup>...>::type {};
        
        template<typename F, typename Tup>
        using is_nothrow_tuple_applyable_ = meta::_t<is_nothrow_tuple_applyable_impl<F, Tup>>;
    }
    
    template<typename F, typename Tup>
    struct is_nothrow_tuple_applyable : std::false_type {};
    
    template<typename F, TupleLike Tup>
    struct is_nothrow_tuple_applyable<F, Tup> : detail::is_nothrow_tuple_applyable_<F, Tup> {};
    
    //////////
    // APPLY
    //////////
    struct apply_fn {
    private:
        template<typename F, TupleLike Tup, std::size_t... Is>
        static constexpr decltype(auto) impl(F&& f, Tup&& tup, meta::iseq<Is...>)
            noexcept(is_nothrow_tuple_applyable<F&&, Tup&&>::value)
        { return ::fcc::invoke(std::forward<F>(f), get<Is>(std::forward<Tup>(tup))...); }
        
    public:
        template<typename F, TupleLike Tup>
            requires TupleApplyable<F&&, Tup&&>()
        constexpr decltype(auto) operator()(F&& f, Tup&& tup) const
            noexcept(is_nothrow_tuple_applyable<F&&, Tup&&>::value)
        { return impl(std::forward<F>(f), std::forward<Tup>(tup), tuple_indices<Tup>{}); }
    };
    namespace { constexpr auto&& apply = static_const<apply_fn>::value; }
    
    /////////
    // JOIN
    /////////
    struct join_fn {
        template<TupleLike Tup>
        constexpr auto operator()(Tup&& tup) const
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            apply(tuple_cat, std::forward<Tup>(tup))
        )
    };
    namespace { constexpr auto&& join = static_const<join_fn>::value; }
    
    //////////////
    // REPLICATE
    //////////////
    template<std::size_t I>
    struct replicate_fn {
    private:
        template<typename T, std::size_t... Is, std::size_t N = sizeof...(Is)>
        static constexpr auto impl(T&& t, meta::iseq<Is...>)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            make_tuple(detail::forward_last<T, Is, N>(t)...)
        )
    
    public:
        template<typename T>
        constexpr auto operator()(T&& t) const
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            impl(std::forward<T>(t), meta::make_iseq<I>{})
        )
    };
    namespace
    {
        template<std::size_t I>
        constexpr auto&& replicate = static_const<replicate_fn<I>>::value;
    }
    
    namespace detail {
        template<typename F, typename Tup, typename ISeq = tuple_indices<Tup>>
        struct is_tuple_for_each_elemable_;
        
        template<typename F, typename Tup, std::size_t... Is>
        struct is_tuple_for_each_elemable_<F, Tup, meta::iseq<Is...>>
            : meta::and_c<
                is_invokable<
                    std::conditional_t<
                        Is == size<Tup>::value - 1,
                        F&&,
                        F&>,
                    get_type<Is, Tup>>::value...> {};
    }
    
    template<typename F, typename Tup>
    struct is_tuple_for_each_elemable : std::false_type {};
    
    template<typename F, TupleLike Tup>
    struct is_tuple_for_each_elemable<F, Tup>
        : detail::is_tuple_for_each_elemable_<F, Tup>::type {};
    
    template<typename F, typename Tup>
    concept bool TupleForEachElemable()
    { return is_tuple_for_each_elemable<F, Tup>::value; }
        
    namespace detail {
        template<typename F, typename Tup, typename ISeq = tuple_indices<Tup>>
        struct is_nothrow_tuple_for_each_elemable_;
        
        template<typename F, typename Tup, std::size_t... Is>
        struct is_nothrow_tuple_for_each_elemable_<F, Tup, meta::iseq<Is...>>
            : meta::and_c<is_nothrow_invokable<F&, get_type<Is, Tup>>::value...> {};
    }
    
    template<typename F, typename Tup>
    struct is_nothrow_tuple_for_each_elemable : std::false_type {};
    
    template<typename F, TupleLike Tup>
    struct is_nothrow_tuple_for_each_elemable<F, Tup>
        : detail::is_nothrow_tuple_for_each_elemable_<F, Tup>::type {};
    
    //////////////////
    // FOR_EACH_ELEM
    //////////////////
    struct for_each_elem_fn {
    private:
        template<typename F, TupleLike Tup, std::size_t... Is, std::size_t N = sizeof...(Is)>
        static constexpr void impl(F&& f, Tup&& tup, meta::iseq<Is...>)
            noexcept(is_nothrow_tuple_for_each_elemable<F&&, Tup&&>::value)
        {
            return (void(::fcc::invoke(detail::forward_last<F, Is, N>(f),
                                       get<Is>(std::forward<Tup>(tup)))) , ...);
        }
    
    public:
        template<typename F, TupleLike Tup>
            requires TupleForEachElemable<F&&, Tup&&>()
        constexpr void operator()(F&& f, Tup&& tup) const
            noexcept(is_nothrow_tuple_for_each_elemable<F&&, Tup&&>::value)
        { return impl(std::forward<F>(f), std::forward<Tup>(tup), tuple_indices<Tup>{}); }
    };
    namespace { constexpr auto&& for_each_elem = static_const<for_each_elem_fn>::value; }
    
    namespace detail {
        template<typename... Tups>
        struct iseq_from_first_;
        
        template<>
        struct iseq_from_first_<> : meta::id<meta::iseq<>> {};
        
        template<typename Tup0, typename... Tups>
        struct iseq_from_first_<Tup0, Tups...> : meta::id<tuple_indices<Tup0>> {};
        
        template<typename... Tups>
        using iseq_from_first = meta::_t<iseq_from_first_<Tups...>>;
    }
    
    //////////////////////////////////////////
    // ZIP_WITH (aka variadic transform/map)
    //////////////////////////////////////////
    struct zip_with_fn {
    private:
        template<std::size_t I, typename F, typename... Tups>
        static constexpr auto invoke_with_elems(F&& f, Tups&&... tups)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            ::fcc::invoke(std::forward<F>(f), get<I>(std::forward<Tups>(tups))...)
        )
        
        template<typename F, typename... Tups, std::size_t... Is, std::size_t N = sizeof...(Is)>
        static constexpr auto impl(F&& f, meta::iseq<Is...>, Tups&&... tups)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            ::fcc::tuple<
                detail::make_tuple_elem_type<
                    decltype(invoke_with_elems<Is>(detail::forward_last<F, Is, N>(f),
                                                   std::forward<Tups>(tups)...))>...>
                {invoke_with_elems<Is>(detail::forward_last<F, Is, N>(f),
                                       std::forward<Tups>(tups)...)...}
        )
        
    public:
        template<typename F, typename... Tups>
            requires meta::and_c<TupleLike<Tups>...>::value &&
                     meta::iseq_all_same<
                        meta::iseq<
                            ::fcc::size<Tups>::value...>>::value
        constexpr auto operator()(F&& f, Tups&&... tups) const
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            impl(std::forward<F>(f), detail::iseq_from_first<Tups...>{}, std::forward<Tups>(tups)...)
        )
    };
    namespace {
        constexpr auto&& zip_with = static_const<zip_with_fn>::value;
        constexpr auto&& map = static_const<zip_with_fn>::value;
    }
    
    ////////
    // ZIP
    ////////
    // tuple_cat style version of zip_with
    struct zip_fn {
    private:
        template<std::size_t I, typename... Tups>
        static constexpr auto join_elems(Tups&&... tups)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            tuple<tuple_element_t<I, std::remove_reference_t<Tups>>...>
                (get<I>(std::forward<Tups>(tups))...)
        )
        
        template<typename... Tups, std::size_t... Is, std::size_t N = sizeof...(Is)>
        static constexpr auto impl(meta::iseq<Is...>, Tups&&... tups)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            ::fcc::tuple<
                decltype(join_elems<Is>(std::forward<Tups>(tups)...))...>
                {join_elems<Is>(std::forward<Tups>(tups)...)...}
        )
        
    public:
        template<typename... Tups>
            requires meta::and_c<TupleLike<Tups>...>::value &&
                     meta::iseq_all_same<
                        meta::iseq<
                            ::fcc::size<Tups>::value...>>::value
        constexpr auto operator()(Tups&&... tups) const
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            impl(detail::iseq_from_first<Tups...>{}, std::forward<Tups>(tups)...)
        )
    };
    namespace { constexpr auto&& zip = static_const<zip_fn>::value; }
    
    ////////////////
    // INTERSPERSE
    ////////////////
    struct intersperse_fn {
    private:
        template<typename Tup>
        using safe_size_minus_1 = meta::size_t<(size<Tup>::value == 0) ? 0 : size<Tup>::value - 1>;
    
    public:
        template<typename T, TupleLike Tup>
        constexpr auto operator()(T&& t, Tup&& tup) const
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            tuple_cat(tuple<tuple_element_t<0, std::remove_reference_t<Tup>>>(head(std::forward<Tup>(tup))),
                      join(zip(replicate<safe_size_minus_1<Tup>::value>(std::forward<T>(t)),
                               tail(std::forward<Tup>(tup)))))
        )
        
        template<typename T, NullTuple Tup>
        constexpr auto operator()(T&&, Tup&& tup) const
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            std::forward<Tup>(tup)
        )
    };
    namespace { constexpr auto&& intersperse = static_const<intersperse_fn>::value; }
    
    //////////
    // FOLDL
    //////////
    struct foldl_fn {
    private:
        template<typename F, typename Init, typename Tup>
        static constexpr auto impl(F&&, Init&& init, Tup&&, meta::iseq<>)
        // this should only be called with empty tuple<>'s
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            std::forward<Init>(init)
        )
        
        template<typename F, typename Init, typename Tup, std::size_t I>
        static constexpr auto impl(F&& f, Init&& init, Tup&& tup, meta::iseq<I>)
        // forward f on last call...
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            ::fcc::invoke(std::forward<F>(f), std::forward<Init>(init),
                          get<I>(std::forward<Tup>(tup)))
        )
        
        template<typename F, typename Init, typename Tup, std::size_t I0, std::size_t I1,
                 std::size_t...Is>
        static constexpr auto impl(F&& f, Init&& init, Tup&& tup, meta::iseq<I0, I1, Is...>)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            impl(std::forward<F>(f),
                 ::fcc::invoke(f, std::forward<Init>(init), get<I0>(std::forward<Tup>(tup))),
                 std::forward<Tup>(tup), meta::iseq<I1, Is...>{})
        )
        
    public:
        template<typename F, typename Init, TupleLike Tup>
        constexpr auto operator()(F&& f, Init&& init, Tup&& tup) const
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            impl(std::forward<F>(f), std::forward<Init>(init), std::forward<Tup>(tup),
                 tuple_indices<Tup>{})
        )
    };
    namespace { constexpr auto foldl = static_const<foldl_fn>::value; }
    
    //////////
    // FOLDR
    //////////
    struct foldr_fn {
    private:
        template<typename F, typename Init, typename Tup>
        static constexpr auto impl(F&&, Init&& init, Tup&&, meta::iseq<>)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            std::forward<Init>(init)
        )
        
        template<typename F, typename Init, typename Tup, std::size_t I0, std::size_t...Is>
            requires (sizeof...(Is) + 1 != size<Tup>::value)
        static constexpr auto impl(F&& f, Init&& init, Tup&& tup, meta::iseq<I0, Is...>)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            ::fcc::invoke(f, get<I0>(std::forward<Tup>(tup)),
                          impl(f, std::forward<Init>(init), std::forward<Tup>(tup),
                               meta::iseq<Is...>{}))
        )
        
        template<typename F, typename Init, typename Tup, std::size_t I0, std::size_t...Is>
            requires (sizeof...(Is) + 1 == size<Tup>::value)
        static constexpr auto impl(F&& f, Init&& init, Tup&& tup, meta::iseq<I0, Is...>)
        // forward f on first call...
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            ::fcc::invoke(std::forward<F>(f), get<I0>(std::forward<Tup>(tup)),
                          impl(f, std::forward<Init>(init), std::forward<Tup>(tup),
                               meta::iseq<Is...>{}))
        )
        
    public:
        template<typename F, typename Init, TupleLike Tup>
        constexpr auto operator()(F&& f, Init&& init, Tup&& tup) const
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            impl(std::forward<F>(f), std::forward<Init>(init), std::forward<Tup>(tup),
                 tuple_indices<Tup>{})
        )
    };
    namespace { constexpr auto foldr = static_const<foldr_fn>::value; }
}

#endif
