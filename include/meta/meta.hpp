// original header prefix
/// \file meta.hpp Tiny meta-programming library.
//
// Meta library
//
//  Copyright Eric Niebler 2014-2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/meta
//

#ifndef META_META_HPP
#define META_META_HPP

#include <type_traits>
#include <utility>

// 99.99999% of this is taken directly, or adapted from,
// eric niebler's <meta/meta.hpp>
// https://github.com/ericniebler/meta
// the goal was to try and bring in only what was needed

namespace meta {
    template<typename T>
    using uncvref = std::remove_cv_t<std::remove_reference_t<T>>;
    
    template <typename T>
    struct always
    {
        template <typename... Ts>
        using apply = T;
    };
    
    template<typename... Ts>
    using void_ = void;
    
    template<typename... Ts>
    struct list {};
    
    template<typename T>
    struct id { using type = T; };
    
    template<typename T>
    using _t = typename T::type;
    
    namespace detail {
        // tweak
        template<typename T, typename = void>
        struct sfinae_t : id<T> {};
        
        template<typename T>
        struct sfinae_t<T, void_<_t<T>>> : id<_t<T>> {};
        
        template <template <typename...> class, typename, typename = void>
        struct defer_
        {
        };

        template <template <typename...> class C, typename... Ts>
        struct defer_<C, list<Ts...>, void_<C<Ts...>>>
        {
            using type = _t<sfinae_t<C<Ts...>>>;
        };

        template <typename T, template <T...> class C, typename, typename = void>
        struct defer_i_
        {
        };

        template <typename T, template <T...> class C, T... Is>
        struct defer_i_<T, C, std::integer_sequence<T, Is...>, void_<C<Is...>>>
        {
            using type = _t<sfinae_t<C<Is...>>>;
        };
    }

    template <template <typename...> class C, typename... Ts>
    struct defer : detail::defer_<C, list<Ts...>>
    {
    };

    template <typename T, template <T...> class C, T... Is>
    struct defer_i : detail::defer_i_<T, C, std::integer_sequence<T, Is...>>
    {
    };
    
    template<typename F, typename... Ts>
    using apply = typename F::template apply<Ts...>;
    
    template <typename... Fs>
    struct compose
    {
    };

    template <typename F0>
    struct compose<F0>
    {
        template <typename... Ts>
        using apply = apply<F0, Ts...>;
    };

    template <typename F0, typename... Fs>
    struct compose<F0, Fs...>
    {
        template <typename... Ts>
        using apply = apply<F0, apply<compose<Fs...>, Ts...>>;
    };
    
    template <template <typename...> class C>
    struct quote
    {
        template <typename... Ts>
        using apply = _t<defer<C, Ts...>>;
    };
    
    template <typename T, template <T...> class F>
    struct quote_i
    {
        template <typename... Ts>
        using apply = _t<defer_i<T, F, Ts::type::value...>>;
    };
    
    template<std::size_t I>
    using size_t = std::integral_constant<std::size_t, I>;
    
    template<bool B>
    using bool_ = std::bool_constant<B>;
    
    struct nil_ {};
    
    using npos = meta::size_t<std::size_t(-1)>;

    namespace detail
    {
        template <typename T>
        constexpr T *_nullptr_v()
        {
            return nullptr;
        }
    }
    
    template<typename T>
    using id_t = _t<id<T>>;
    
    template<bool... Bools>
    using and_c = _t<std::is_same<std::integer_sequence<bool, true, Bools...>,
                                  std::integer_sequence<bool, Bools..., true>>>;
    
    namespace detail {
        template<typename T>
        struct list_size_;
        
        template<typename... Ts>
        struct list_size_<list<Ts...>> : size_t<sizeof...(Ts)> {};
    }
    
    template<typename T>
    using list_size = _t<detail::list_size_<T>>;
    
    namespace detail
    {
        template <typename... Lists>
        struct concat_
        {
        };

        template <>
        struct concat_<>
        {
            using type = list<>;
        };

        template <typename... List1>
        struct concat_<list<List1...>>
        {
            using type = list<List1...>;
        };

        template <typename... List1, typename... List2>
        struct concat_<list<List1...>, list<List2...>>
        {
            using type = list<List1..., List2...>;
        };

        template <typename... List1, typename... List2, typename... List3>
        struct concat_<list<List1...>, list<List2...>, list<List3...>>
        {
            using type = list<List1..., List2..., List3...>;
        };

        template <typename... List1, typename... List2, typename... List3, typename... Rest>
        struct concat_<list<List1...>, list<List2...>, list<List3...>, Rest...>
            : concat_<list<List1..., List2..., List3...>, Rest...>
        {
        };
    }
    
    template <typename... Lists>
    using concat = _t<detail::concat_<Lists...>>;
    
    namespace detail
    {
        template <std::size_t N, typename T>
        struct repeat_n_c_
        {
            using type = concat<_t<repeat_n_c_<N / 2, T>>,
                                _t<repeat_n_c_<N / 2, T>>,
                                _t<repeat_n_c_<N % 2, T>>>;
        };

        template <typename T>
        struct repeat_n_c_<0, T>
        {
            using type = list<>;
        };

        template <typename T>
        struct repeat_n_c_<1, T>
        {
            using type = list<T>;
        };
    }
    
    template <typename N, typename T = void>
    using repeat_n = _t<detail::repeat_n_c_<_t<N>::value, T>>;
    
    template <std::size_t N, typename T = void>
    using repeat_n_c = _t<detail::repeat_n_c_<N, T>>;

    namespace detail
    {
        template <typename VoidPtrs>
        struct at_impl_;

        template <typename... VoidPtrs>
        struct at_impl_<list<VoidPtrs...>>
        {
            static nil_ eval(...);

            template <typename T, typename... Us>
            static T eval(VoidPtrs..., T *, Us *...);
        };

        template <typename List, typename N>
        struct at_
        {
        };

        template <typename... Ts, typename N>
        struct at_<list<Ts...>, N>
            : decltype(at_impl_<repeat_n<N, void *>>::eval(detail::_nullptr_v<id<Ts>>()...))
        {
        };
    }
    
    template <typename List, typename N>
    using at = _t<detail::at_<List, N>>;
    
    template <typename List, std::size_t N>
    using at_c = at<List, meta::size_t<N>>;
    
    namespace detail
    {
        template <typename, typename = void>
        struct transform_
        {
        };

        template <typename... Ts, typename Fun>
        struct transform_<list<list<Ts...>, Fun>, void_<apply<Fun, Ts>...>>
        {
            using type = list<apply<Fun, Ts>...>;
        };

        template <typename... Ts0, typename... Ts1, typename Fun>
        struct transform_<list<list<Ts0...>, list<Ts1...>, Fun>,
                          void_<apply<Fun, Ts0, Ts1>...>>
        {
            using type = list<apply<Fun, Ts0, Ts1>...>;
        };
    }
    
    template <typename... Args>
    using transform = _t<detail::transform_<list<Args...>>>;
    
    namespace detail
    {
        constexpr std::size_t find_index_i_(bool const *const first, bool const *const last,
                                            std::size_t N = 0)
        {
            return first == last ? npos::value : *first ? N
                                                        : find_index_i_(first + 1, last, N + 1);
        }

        template <typename List, typename T>
        struct find_index_
        {
        };

        template <typename V>
        struct find_index_<list<>, V>
        {
            using type = npos;
        };

        template <typename... T, typename V>
        struct find_index_<list<T...>, V>
        {
            static constexpr bool s_v[] = {std::is_same<T, V>::value...};
            using type = size_t<find_index_i_(s_v, s_v + sizeof...(T))>;
        };
    }
    
    template <typename List, typename T>
    using find_index = _t<detail::find_index_<List, T>>;
    
    namespace detail
    {
        constexpr std::size_t reverse_find_index_i_(bool const *const first,
                                                    bool const *const last, std::size_t N)
        {
            return first == last
                       ? npos::value
                       : *(last - 1) ? N - 1 : reverse_find_index_i_(first, last - 1, N - 1);
        }

        template <typename List, typename T>
        struct reverse_find_index_
        {
        };

        template <typename V>
        struct reverse_find_index_<list<>, V>
        {
            using type = npos;
        };

        template <typename... T, typename V>
        struct reverse_find_index_<list<T...>, V>
        {
            static constexpr bool s_v[] = {std::is_same<T, V>::value...};
            using type = size_t<reverse_find_index_i_(s_v, s_v + sizeof...(T), sizeof...(T))>;
        };
    }
    
    template <typename List, typename T>
    using reverse_find_index = _t<detail::reverse_find_index_<List, T>>;
    
    namespace detail
    {
        template <typename F, typename List>
        struct apply_list
        {};

        template <typename F, typename Ret, typename... Args>
        struct apply_list<F, Ret(Args...)> : id<apply<F, Ret, Args...>>
        {};

        template <typename F, template <typename...> class T, typename... Ts>
        struct apply_list<F, T<Ts...>> : id<apply<F, Ts...>>
        {};

        template <typename F, typename T, T... Is>
        struct apply_list<F, std::integer_sequence<T, Is...>>
            : id<apply<F, std::integral_constant<T, Is>...>>
        {};
    }
    
    template <typename C, typename List>
    using apply_list = _t<detail::apply_list<C, List>>;
    
    template <typename ListOfLists>
    using join = apply_list<quote<concat>, ListOfLists>;
    
    template <typename F, typename... Ts>
    struct bind_front
    {
        template <typename... Us>
        using apply = apply<F, Ts..., Us...>;
    };
    
    template <typename F, typename... Us>
    struct bind_back
    {
        template <typename... Ts>
        using apply = apply<F, Ts..., Us...>;
    };
    
    template<std::size_t I, std::size_t J>
    using min_c = size_t<(I < J) ? I : J>;
    template<std::size_t N>
    using make_iseq = std::make_index_sequence<N>;
    
    template<std::size_t... Is>
    using iseq = std::integer_sequence<std::size_t, Is...>;
    
    namespace detail {
        template<typename ISeq>
        struct iseq_as_list_;
        template<std::size_t... Is>
        struct iseq_as_list_<meta::iseq<Is...>>
            : id<list<size_t<Is>...>> {};
    }
    
    template<typename ISeq>
    using iseq_as_list = _t<detail::iseq_as_list_<ISeq>>;
    
    namespace detail {
        template<typename ISeq>
        struct iseq_all_same_;
        
        template<>
        struct iseq_all_same_<iseq<>> : std::true_type {};
        
        template<std::size_t I, std::size_t... Is>
        struct iseq_all_same_<iseq<I, Is...>>
            : std::is_same<iseq<I, Is...>, iseq<Is..., I>>::type {};
    }
    
    template<typename ISeq>
    using iseq_all_same = _t<detail::iseq_all_same_<ISeq>>;
    
    namespace detail {
        template<typename ISeq, std::size_t Offset>
        struct offset_iseq_;
        
        template<std::size_t... Is, std::size_t Offset>
        struct offset_iseq_<iseq<Is...>, Offset>
        { using type = iseq<(Is + Offset)...>; };
    }
    
    template<std::size_t N, std::size_t Offset>
    using offset_iseq = _t<detail::offset_iseq_<make_iseq<N>, Offset>>;
    
    template<typename T, typename As>
    using as_same_const = std::conditional_t<
        std::is_const<As>::value,
        std::add_const_t<T>,
        T>;
    
    template<typename T, typename As>
    using as_same_volatile = std::conditional_t<
        std::is_volatile<As>::value,
        std::add_volatile_t<T>,
        T>;
    
    template<typename T, typename As>
    using as_same_cv = as_same_const<as_same_volatile<T, As>, As>;
    
    // no-ref=rvalue ref
    template<typename T, typename As>
    using as_same_ref = std::conditional_t<
        std::is_lvalue_reference<As>::value,
        std::add_lvalue_reference_t<T>,
        std::add_rvalue_reference_t<T>>;
    
    template<typename T, typename As>
    using as_same_cvref =
        as_same_ref<
            as_same_cv<T, std::remove_reference_t<As>>,
            As>;
}

#endif
