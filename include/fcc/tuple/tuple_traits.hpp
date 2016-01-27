//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_TUPLE_TUPLE_TRAITS_HPP
#define FCC_TUPLE_TUPLE_TRAITS_HPP

#include <fcc/tuple/tuple_element_access.hpp>

#include <fcc/detail/generic_traits.hpp>

#include <fcc/invoke.hpp>

#include <tuple>

namespace fcc {
    ///////////////
    // is_n_tuple
    ///////////////
    template<std::size_t N, typename T>
    struct is_n_tuple : std::false_type {};
    
    template<std::size_t N, TupleLike T>
        requires size<T>::value == N
    struct is_n_tuple<N, T> : std::true_type {};
    
    template<std::size_t N, typename T>
    concept bool NTuple = TupleLike<T> && is_n_tuple<N, T>::value;

    //////////////////
    // is_null_tuple
    //////////////////
    template<typename T>
    struct is_null_tuple : is_n_tuple<0, T>::type {};
    
    template<typename T>
    concept bool NullTuple = NTuple<0, T>;

    ///////////////////////
    // is_singleton_tuple
    ///////////////////////
    // redundant name - but less confusing, and more consistent
    template<typename T>
    struct is_singleton_tuple : is_n_tuple<1, T>::type {};
    
    template<typename T>
    concept bool SingletonTuple = NTuple<1, T>;

    //////////////////
    // is_pair_tuple
    //////////////////
    // redundant name - but less confusing, and more consistent
    template<typename T>
    struct is_pair_tuple : is_n_tuple<2, T>::type {};
    
    template<typename T>
    concept bool PairTuple = NTuple<2, T>;

    namespace detail {
        // constructible template. used for all convertible/constructible etc concepts/traits
        template<typename ConTrait, typename ConISeq, typename DefConISeq, TupleLike Tup0,
                 typename... Us>
        struct construct_template_impl;
        
        template<typename ConTrait, std::size_t... Cs, std::size_t... Ds, TupleLike Tup0,
                 typename... Us>
        struct construct_template_impl<ConTrait, meta::iseq<Cs...>, meta::iseq<Ds...>, Tup0, Us...>
            : meta::and_c<
                meta::apply<ConTrait, tuple_element_t<Cs, Tup0>, Us>{}...,
                meta::apply<ConTrait, tuple_element_t<Ds, Tup0>>{}...>
        {};
        
        template<typename ConTrait, TupleLike Tup0, typename... Us>
        struct construct_template_sized
            : construct_template_impl<
                ConTrait,
                meta::make_iseq<sizeof...(Us)>,
                meta::offset_iseq<tuple_size<Tup0>{} - sizeof...(Us), sizeof...(Us)>,
                Tup0,
                Us...>
        {};
        
        template<typename ConTrait, typename Tup0, typename... Us>
        struct construct_template_ : std::false_type
        {
            static_assert(std::is_same<std::decay_t<Tup0>, Tup0>{},
                          "Type to construct must have no cvref qualifiers");
        };
        
        template<typename ConTrait, UnqualifiedTupleLike Tup0, typename... Us>
            requires sizeof...(Us) <= tuple_size<Tup0>{}
        struct construct_template_<ConTrait, Tup0, Us...>
            : construct_template_sized<ConTrait, Tup0, Us...> {};
        
        template<typename ConTrait, typename Tup0, typename... Us>
        using construct_template = meta::_t<construct_template_<ConTrait, Tup0, Us...>>;
    }
    
    ///////////////////////////////
    // tuple(Us&&...) constructor
    ///////////////////////////////
    template<typename Tup0, typename... Us>
    using is_tuple_constructible =
        detail::construct_template<meta::quote<std::is_constructible>, Tup0, Us...>;
    
    template<typename T, typename... Us>
    concept bool TupleConstructible = is_tuple_constructible<T, Us...>::value;
    
    //////////////////////////////////////////////////////////////
    // tuple(allocator_arg_t, const Alloc&, Us&&...) constructor
    //////////////////////////////////////////////////////////////
    template<typename Tup0, typename Alloc, typename... Us>
    using is_tuple_alloc_constructible =
        detail::construct_template<bound_uses_alloc_constructible<Alloc>, Tup0, Us...>;
    
    template<typename T, typename Alloc, typename... Us>
    concept bool TupleAllocConstructible = is_tuple_alloc_constructible<T, Alloc, Us...>::value;
    
    ////////////////////////////////////////
    // tuple(Us&&...) noexcept constructor
    ////////////////////////////////////////
    template<typename Tup0, typename... Us>
    using is_nothrow_tuple_constructible =
        detail::construct_template<meta::quote<std::is_nothrow_constructible>, Tup0, Us...>;
    
    ///////////////////////////////////////////////////////////////////////
    // tuple(allocator_arg_t, const Alloc&, Us&&...) noexcept constructor
    ///////////////////////////////////////////////////////////////////////
    template<typename Tup0, typename Alloc, typename... Us>
    using is_nothrow_tuple_alloc_constructible =
        detail::construct_template<bound_nothrow_uses_alloc_constructible<Alloc>, Tup0, Us...>;
    
    /////////////////////////////////////
    // tuple(Us&&...) convertible trait
    /////////////////////////////////////
    template<typename Tup0, typename... Us>
    using is_tuple_convertible =
        detail::construct_template<meta::quote<is_convertible>, Tup0, Us...>;
    
    template<typename Tup0, typename... Us>
    concept bool TupleConvertible = is_tuple_convertible<Tup0, Us...>::value;
    
    ////////////////////////////////////////////////////////////////////
    // tuple(allocator_arg_t, const Alloc&, Us&&...) convertible trait
    ////////////////////////////////////////////////////////////////////
    template<typename Tup0, typename Alloc, typename... Us>
    using is_tuple_alloc_convertible =
        detail::construct_template<bound_uses_alloc_convertible<Alloc>, Tup0, Us...>;
    
    template<typename Tup0, typename Alloc, typename... Us>
    concept bool TupleAllocConvertible = is_tuple_alloc_convertible<Tup0, Alloc, Us...>::value;
    
    namespace detail {
        // tuple constructible template. used for construction from TupleLike types
        template<typename ConTrait, TupleLike Tup0, TupleLike Tup1, typename = tuple_indices<Tup0>>
        struct tuple_constructible_template_sized : std::false_type {};

        template<typename ConTrait, TupleLike Tup0, TupleLike Tup1, std::size_t... Is>
        struct tuple_constructible_template_sized<ConTrait, Tup0, Tup1, meta::iseq<Is...>>
            : construct_template<ConTrait, Tup0, get_type<Is, Tup1>...> {};

        template<typename ConTrait, typename Tup0, typename Tup1>
        struct tuple_constructible_template_ : std::false_type
        {
            static_assert(std::is_same<std::decay_t<Tup0>, Tup0>{},
                          "Type to construct must have no cvref qualifiers");
        };

        template<typename ConTrait, UnqualifiedTupleLike Tup0, TupleLike Tup1>
            requires tuple_size<Tup0>{} == size<Tup1>{}
        struct tuple_constructible_template_<ConTrait, Tup0, Tup1>
            : tuple_constructible_template_sized<ConTrait, Tup0, Tup1> {};

        template<typename ConTrait, typename Tup0, typename Tup1>
        using tuple_constructible_template = meta::_t<tuple_constructible_template_<ConTrait,
                                                                                    Tup0,
                                                                                    Tup1>>;
    }
    
    ///////////////////////////////////
    // tuple(TupleLike&&) constructor
    ///////////////////////////////////
    template<typename Tup0, typename Tup1>
    using is_tuple_tuple_constructible =
        detail::tuple_constructible_template<meta::quote<std::is_constructible>, Tup0, Tup1>;
    
    template<typename Tup0, typename Tup1>
    concept bool TupleTupleConstructible = is_tuple_tuple_constructible<Tup0, Tup1>::value;
    
    //////////////////////////////////////////////////////////////////
    // tuple(allocator_arg_t, const Alloc&, TupleLike&&) constructor
    //////////////////////////////////////////////////////////////////
    template<typename Tup0, typename Alloc, typename Tup1>
    using is_tuple_tuple_alloc_constructible =
        detail::tuple_constructible_template<bound_uses_alloc_constructible<Alloc>, Tup0, Tup1>;
    
    template<typename Tup0, typename Alloc, typename Tup1>
    concept bool TupleTupleAllocConstructible =
        is_tuple_tuple_alloc_constructible<Tup0, Alloc, Tup1>::value;
    
    ////////////////////////////////////////////
    // tuple(TupleLike&&) noexcept constructor
    ////////////////////////////////////////////
    template<typename Tup0, typename Tup1>
    using is_nothrow_tuple_tuple_constructible =
        detail::tuple_constructible_template<meta::quote<std::is_nothrow_constructible>, Tup0,
                                             Tup1>;
    
    ///////////////////////////////////////////////////////////////////////////
    // tuple(allocator_arg_t, const Alloc&, TupleLike&&) noexcept constructor
    ///////////////////////////////////////////////////////////////////////////
    template<typename Tup0, typename Alloc, typename Tup1>
    using is_nothrow_tuple_tuple_alloc_constructible =
        detail::tuple_constructible_template<bound_nothrow_uses_alloc_constructible<Alloc>, Tup0,
                                             Tup1>;
    
    /////////////////////////////////////////
    // tuple(TupleLike&&) convertible trait
    /////////////////////////////////////////
    template<typename Tup0, typename Tup1>
    using is_tuple_tuple_convertible =
        detail::tuple_constructible_template<meta::quote<is_convertible>, Tup0, Tup1>;
    
    template<typename Tup0, typename Tup1>
    concept bool TupleTupleConvertible = is_tuple_tuple_convertible<Tup0, Tup1>::value;
    
    ////////////////////////////////////////////////////////////////////////
    // tuple(allocator_arg_t, const Alloc&, TupleLike&&) convertible trait
    ////////////////////////////////////////////////////////////////////////
    template<typename Tup0, typename Alloc, typename Tup1>
    using is_tuple_tuple_alloc_convertible =
        detail::tuple_constructible_template<bound_uses_alloc_convertible<Alloc>, Tup0, Tup1>;
    
    template<typename Tup0, typename Alloc, typename Tup1>
    concept bool TupleTupleAllocConvertible =
        is_tuple_tuple_alloc_convertible<Tup0, Alloc, Tup1>::value;
    
    namespace detail {
        // piecewise constructible template. used for all piecewise_construct_t concepts/traits
        template<typename ConTrait, typename T, TupleLike UTuple,
                 typename ISeq = tuple_indices<UTuple>>
        struct constructible_from_tuple_gets;
        
        template<typename ConTrait, typename T, TupleLike UTuple, std::size_t... Is>
        struct constructible_from_tuple_gets<ConTrait, T, UTuple, meta::iseq<Is...>>
            : meta::apply<ConTrait, T, get_type<Is, UTuple>...>::type {};
        
        template<typename ConTrait, typename ConISeq, typename DefConISeq, TupleLike Tup0,
                 TupleLike... UTuples>
        struct piecewise_construct_template_impl;
        
        template<typename ConTrait, std::size_t... Cs, std::size_t... Ds, TupleLike Tup0,
                 TupleLike... UTuples>
        struct piecewise_construct_template_impl<ConTrait, meta::iseq<Cs...>, meta::iseq<Ds...>,
                                                 Tup0, UTuples...>
            : meta::and_c<
                constructible_from_tuple_gets<ConTrait, tuple_element_t<Cs, Tup0>, UTuples>{}...,
                meta::apply<ConTrait, tuple_element_t<Ds, Tup0>>{}...>
        {};
        
        template<typename ConTrait, TupleLike Tup0, TupleLike... UTuples>
        struct piecewise_construct_template_sized
            : piecewise_construct_template_impl<
                ConTrait,
                meta::make_iseq<sizeof...(UTuples)>,
                meta::offset_iseq<tuple_size<Tup0>{} - sizeof...(UTuples), sizeof...(UTuples)>,
                Tup0,
                UTuples...>
        {};
        
        template<typename ConTrait, typename Tup0, typename... UTuples>
        struct piecewise_construct_template_ : std::false_type
        {
            static_assert(std::is_same<std::decay_t<Tup0>, Tup0>{},
                          "Type to construct must have no cvref qualifiers");
        };
        
        template<typename ConTrait, UnqualifiedTupleLike Tup0, TupleLike... UTuples>
            requires sizeof...(UTuples) <= tuple_size<Tup0>{}
        struct piecewise_construct_template_<ConTrait, Tup0, UTuples...>
            : piecewise_construct_template_sized<ConTrait, Tup0, UTuples...> {};
        
        template<typename ConTrait, typename Tup0, typename... UTuples>
        using piecewise_construct_template =
            meta::_t<piecewise_construct_template_<ConTrait, Tup0, UTuples...>>;
    }
    
    /////////////////////////////////////////////////////////////
    // tuple(piecewise_construct_t, TupleLike&&...) constructor
    /////////////////////////////////////////////////////////////
    template<typename Tup0, typename... UTuples>
    using is_piecewise_tuple_constructible =
        detail::piecewise_construct_template<meta::quote<std::is_constructible>, Tup0, UTuples...>;
    
    template<typename T, typename... UTuples>
    concept bool TuplePiecewiseConstructible = is_piecewise_tuple_constructible<T, UTuples...>{}();
    
    ////////////////////////////////////////////////////////////////////////////////////////////
    // tuple(piecewise_construct_t, allocator_arg_t, const Alloc&, TupleLike&&...) constructor
    ////////////////////////////////////////////////////////////////////////////////////////////
    template<typename Tup0, typename Alloc, typename... UTuples>
    using is_piecewise_tuple_alloc_constructible =
        detail::piecewise_construct_template<bound_uses_alloc_constructible<Alloc>, Tup0,
                                             UTuples...>;
    
    template<typename T, typename Alloc, typename... UTuples>
    concept bool TuplePiecewiseAllocConstructible =
        is_tuple_alloc_constructible<T, Alloc, UTuples...>::value;
    
    //////////////////////////////////////////////////////////////////////
    // tuple(piecewise_construct_t, TupleLike&&...) noexcept constructor
    //////////////////////////////////////////////////////////////////////
    template<typename Tup0, typename... UTuples>
    using is_nothrow_piecewise_tuple_constructible =
        detail::piecewise_construct_template<meta::quote<std::is_nothrow_constructible>, Tup0,
                                             UTuples...>;
    
    /////////////////////////////////////////////////////////////////////////////////////////
    // tuple(piecewise_construct_t, allocator_arg_t, const Alloc&, TupleLike&&...) noexcept
    // constructor
    /////////////////////////////////////////////////////////////////////////////////////////
    template<typename Tup0, typename Alloc, typename... UTuples>
    using is_nothrow_piecewise_tuple_alloc_constructible =
        detail::piecewise_construct_template<bound_nothrow_uses_alloc_constructible<Alloc>, Tup0,
                                             UTuples...>;
    
    ///////////////////////////////////////////////////////////////////
    // tuple(piecewise_construct_t, TupleLike&&...) convertible trait
    ///////////////////////////////////////////////////////////////////
    template<typename Tup0, typename... UTuples>
    using is_piecewise_tuple_convertible =
        detail::piecewise_construct_template<meta::quote<is_convertible>, Tup0, UTuples...>;
    
    template<typename Tup0, typename... UTuples>
    concept bool TuplePiecewiseConvertible = is_piecewise_tuple_convertible<Tup0, UTuples...>{}();
    
    ////////////////////////////////////////////////////////////////////////////////////////////
    // tuple(piecewise_construct_t, allocator_arg_t, const Alloc&, TupleLike&&...) convertible
    // trait
    ////////////////////////////////////////////////////////////////////////////////////////////
    template<typename Tup0, typename Alloc, typename... UTuples>
    using is_piecewise_tuple_alloc_convertible =
        detail::piecewise_construct_template<bound_uses_alloc_convertible<Alloc>, Tup0, UTuples...>;
    
    template<typename Tup0, typename Alloc, typename... UTuples>
    concept bool TuplePiecewiseAllocConvertible =
        is_piecewise_tuple_alloc_convertible<Tup0, Alloc, UTuples...>::value;
    
    namespace detail {
        // binary get_type based template. used for all templates that depend on the
        // get_type<Is, Tuple> of both arguments
        template<typename GetBasedTrait, TupleLike Tup0, TupleLike Tup1,
                 typename = tuple_indices<Tup0>>
        struct tuple_get_based_trait_template_impl;

        template<typename GetBasedTrait, TupleLike Tup0, TupleLike Tup1, std::size_t... Is>
        struct tuple_get_based_trait_template_impl<GetBasedTrait, Tup0, Tup1, meta::iseq<Is...>>
            : meta::and_c<meta::apply<GetBasedTrait, get_type<Is, Tup0>, get_type<Is, Tup1>>{}...>
        {};

        template<typename GetBasedTrait, typename Tup0, typename Tup1>
        struct tuple_get_based_trait_template_ : std::false_type {};

        template<typename GetBasedTrait, TupleLike Tup0, TupleLike Tup1>
            requires size<Tup0>::value == size<Tup1>::value
        struct tuple_get_based_trait_template_<GetBasedTrait, Tup0, Tup1>
            : tuple_get_based_trait_template_impl<GetBasedTrait, Tup0, Tup1> {};
        
        template<typename GetBasedTrait, typename Tup0, typename Tup1>
        using tuple_get_based_trait_template =
            meta::_t<tuple_get_based_trait_template_<GetBasedTrait, Tup0, Tup1>>;
    }
    
    ///////////////////////////
    // operator=(TupleLike&&)
    ///////////////////////////
    template<typename Tup0, typename Tup1>
    using is_tuple_tuple_assignable =
        detail::tuple_get_based_trait_template<meta::quote<std::is_assignable>, Tup0, Tup1>;
    
    template<typename Tup0, typename Tup1>
    concept bool TupleTupleAssignable = is_tuple_tuple_assignable<Tup0, Tup1>::value;
    
    ////////////////////////////////////
    // operator=(TupleLike&&) noexcept
    ////////////////////////////////////
    template<typename Tup0, typename Tup1>
    using is_nothrow_tuple_tuple_assignable =
        detail::tuple_get_based_trait_template<meta::quote<std::is_nothrow_assignable>, Tup0, Tup1>;
    
    namespace detail {
        // if your get based trait requires commutativity, use this!
        template<typename GetBasedTrait, typename Tup0, typename Tup1>
        using commutative_get_based_trait = meta::bool_<
            tuple_get_based_trait_template<GetBasedTrait, Tup0, Tup1>::value &&
            tuple_get_based_trait_template<GetBasedTrait, Tup1, Tup0>::value>;
    }
}

#endif
