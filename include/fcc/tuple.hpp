//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_TUPLE_HPP
#define FCC_TUPLE_HPP

#include <fcc/tuple/tuple_impl.hpp>

#include <fcc/tuple/tuple_algorithm.hpp>
#include <fcc/tuple/tuple_io.hpp>
#include <fcc/tuple/tuple_operator.hpp>

#include <fcc/detail/literals.hpp>

namespace fcc {    
    namespace detail { struct tuple_access; }
    
    /////////////
    // FccTuple
    /////////////
    namespace detail {
        template<typename T>
        struct is_fcc_tuple_ : std::false_type {};
        
        template<typename... Ts>
        struct is_fcc_tuple_<::fcc::tuple<Ts...>> : std::true_type {};
    }
    
    template<typename T>
    struct is_fcc_tuple : detail::is_fcc_tuple_<meta::uncvref<T>>::type {};
    
    template<typename T>
    concept bool FccTuple = is_fcc_tuple<T>::value;
    
    template<std::size_t I, FccTuple Tup> requires (I < size<Tup>::value)
    constexpr get_type<I, Tup> get(Tup&& tup) noexcept;
    
    template<typename... Ts, Allocator Alloc>
    struct uses_allocator<::fcc::tuple<Ts...>, Alloc> : std::true_type {};
    
    //////////
    // TUPLE
    //////////
    template<typename... Ts>
    struct tuple : private detail::tuple_base<meta::make_iseq<sizeof...(Ts)>, Ts...>
    {
    private:
        using base = detail::tuple_base<meta::make_iseq<sizeof...(Ts)>, Ts...>;
        
        // friend
        friend detail::tuple_access;
    
    public:
        // no defaults, and don't participate in overload resolution
        tuple(const tuple& rhs)            requires false = delete;
        tuple(tuple&& rhs)                 requires false = delete;
        tuple& operator=(const tuple& rhs) requires false = delete;
        tuple& operator=(tuple&& rhs)      requires false = delete;
        
        /////////////////////////////
        // BRACED INIT CONSTRUCTORS
        /////////////////////////////
        
        // braced init copy constructor
        constexpr tuple(braced_init_type<Ts>... ts)
            noexcept(is_nothrow_tuple_constructible<tuple, braced_init_type<Ts>...>::value)
            requires TupleConstructible<tuple, braced_init_type<Ts>...> &&
                     TupleConvertible<tuple, braced_init_type<Ts>...>
            : base(detail::initialize, meta::make_iseq<sizeof...(Ts)>{}, meta::iseq<>{},
                   std::forward<Ts>(ts)...)
        {}
        
        explicit constexpr tuple(braced_init_type<Ts>... ts)
            noexcept(is_nothrow_tuple_constructible<tuple, braced_init_type<Ts>...>::value)
            requires TupleConstructible<tuple, braced_init_type<Ts>...> &&
                     !TupleConvertible<tuple, braced_init_type<Ts>...>
            : base(detail::initialize, meta::make_iseq<sizeof...(Ts)>{}, meta::iseq<>{},
                   std::forward<Ts>(ts)...)
        {}
        
        // braced init allocator copy constructor
        template<Allocator Alloc>
            requires TupleAllocConstructible<tuple, const Alloc&, braced_init_type<Ts>...> &&
                     TupleAllocConvertible<tuple, const Alloc&, braced_init_type<Ts>...>
        constexpr tuple(allocator_arg_t, const Alloc& alloc, braced_init_type<Ts>... ts)
            noexcept(is_nothrow_tuple_alloc_constructible<tuple, const Alloc&,
                                                          braced_init_type<Ts>...>::value)
            : base(allocator_arg, alloc, meta::make_iseq<sizeof...(Ts)>{}, meta::iseq<>{},
                   std::forward<Ts>(ts)...)
        {}
        
        template<Allocator Alloc>
            requires TupleAllocConstructible<tuple, const Alloc&, braced_init_type<Ts>...> &&
                     !TupleAllocConvertible<tuple, const Alloc&, braced_init_type<Ts>...>
        explicit constexpr tuple(allocator_arg_t, const Alloc& alloc, braced_init_type<Ts>... ts)
            noexcept(is_nothrow_tuple_alloc_constructible<tuple, const Alloc&,
                                                          braced_init_type<Ts>...>::value)
            : base(allocator_arg, alloc, meta::make_iseq<sizeof...(Ts)>{}, meta::iseq<>{},
                   std::forward<Ts>(ts)...)
        {}
        
        ////////////////////////////////////
        // PERFECT FORWARDING CONSTRUCTORS
        ////////////////////////////////////
        
        // forwarding constructor
        template<typename... Us>
            requires TupleConstructible<tuple, Us&&...> &&
                     TupleConvertible<tuple, Us&&...>
        constexpr tuple(Us&&... us)
            noexcept(is_nothrow_tuple_constructible<tuple, Us&&...>::value)
            : base(detail::initialize, meta::make_iseq<sizeof...(Us)>{},
                   meta::offset_iseq<sizeof...(Ts) - sizeof...(Us), sizeof...(Us)>{},
                   std::forward<Us>(us)...)
        {}
        
        template<typename... Us>
            requires TupleConstructible<tuple, Us&&...> &&
                     !TupleConvertible<tuple, Us&&...>
        explicit constexpr tuple(Us&&... us)
            noexcept(is_nothrow_tuple_constructible<tuple, Us&&...>::value)
            : base(detail::initialize, meta::make_iseq<sizeof...(Us)>{},
                   meta::offset_iseq<sizeof...(Ts) - sizeof...(Us), sizeof...(Us)>{},
                   std::forward<Us>(us)...)
        {}
        
        // allocator forwarding constructor
        template<Allocator Alloc, typename... Us>
            requires TupleAllocConstructible<tuple, const Alloc&, Us&&...> &&
                     TupleAllocConvertible<tuple, const Alloc&, Us&&...>
        constexpr tuple(allocator_arg_t, const Alloc& alloc, Us&&... us)
            noexcept(is_nothrow_tuple_alloc_constructible<tuple, const Alloc&, Us&&...>::value)
            : base(allocator_arg, alloc, meta::make_iseq<sizeof...(Us)>{},
                   meta::offset_iseq<sizeof...(Ts) - sizeof...(Us), sizeof...(Us)>{},
                   std::forward<Us>(us)...)
        {}
        
        template<Allocator Alloc, typename... Us>
            requires TupleAllocConstructible<tuple, const Alloc&, Us&&...> &&
                     !TupleAllocConvertible<tuple, const Alloc&, Us&&...>
        explicit constexpr tuple(allocator_arg_t, const Alloc& alloc, Us&&... us)
            noexcept(is_nothrow_tuple_alloc_constructible<tuple, const Alloc&, Us&&...>::value)
            : base(allocator_arg, alloc, meta::make_iseq<sizeof...(Us)>{},
                   meta::offset_iseq<sizeof...(Ts) - sizeof...(Us), sizeof...(Us)>{},
                   std::forward<Us>(us)...)
        {}
        
        //////////////////////////////////////////
        // TUPLE PERFECT FORWARDING CONSTRUCTORS
        //////////////////////////////////////////

        // tuple forwarding constructor
        template<TupleLike UTuple>
            requires TupleTupleConstructible<tuple, UTuple> &&
                     TupleTupleConvertible<tuple, UTuple>
        constexpr tuple(UTuple&& rhs)
            noexcept(is_nothrow_tuple_tuple_constructible<tuple, UTuple>::value)
            : base(detail::tuple_initialize, std::forward<UTuple>(rhs))
        {}
        
        template<TupleLike UTuple>
            requires TupleTupleConstructible<tuple, UTuple> &&
                     !TupleTupleConvertible<tuple, UTuple>
        explicit constexpr tuple(UTuple&& rhs)
            noexcept(is_nothrow_tuple_tuple_constructible<tuple, UTuple>::value)
            : base(detail::tuple_initialize, std::forward<UTuple>(rhs))
        {}
        
        // tuple allocator forwarding constructor
        template<Allocator Alloc, TupleLike UTuple>
            requires TupleTupleAllocConstructible<tuple, const Alloc&, UTuple> &&
                     TupleTupleAllocConvertible<tuple, const Alloc&, UTuple>
        constexpr tuple(allocator_arg_t, const Alloc& alloc, UTuple&& rhs)
            noexcept(is_nothrow_tuple_tuple_alloc_constructible<tuple, const Alloc&, UTuple>::value)
            : base(detail::tuple_initialize, allocator_arg, alloc, std::forward<UTuple>(rhs))
        {}
        
        template<Allocator Alloc, TupleLike UTuple>
            requires TupleTupleAllocConstructible<tuple, const Alloc&, UTuple> &&
                     !TupleTupleAllocConvertible<tuple, const Alloc&, UTuple>
        explicit constexpr tuple(allocator_arg_t, const Alloc& alloc, UTuple&& rhs)
            noexcept(is_nothrow_tuple_tuple_alloc_constructible<tuple, const Alloc&, UTuple>::value)
            : base(detail::tuple_initialize, allocator_arg, alloc, std::forward<UTuple>(rhs))
        {}
        
        //////////////////////////////////////////////
        // PIECEWISE PERFECT FORWARDING CONSTRUCTORS
        //////////////////////////////////////////////
        
        // piecewise forwarding constructor
        template<typename... UTuples>
            requires TuplePiecewiseConstructible<tuple, UTuples&&...> &&
                     TuplePiecewiseConvertible<tuple, UTuples&&...>
        constexpr tuple(piecewise_construct_t, UTuples&&... utuples)
            noexcept(is_nothrow_piecewise_tuple_constructible<tuple, UTuples&&...>::value)
            : base(piecewise_construct, meta::make_iseq<sizeof...(UTuples)>{},
                   meta::offset_iseq<sizeof...(Ts) - sizeof...(UTuples), sizeof...(UTuples)>{},
                   std::forward<UTuples>(utuples)...)
        {}
        
        template<typename... UTuples>
            requires TuplePiecewiseConstructible<tuple, UTuples&&...> &&
                     !TuplePiecewiseConvertible<tuple, UTuples&&...>
        explicit constexpr tuple(piecewise_construct_t, UTuples&&... utuples)
            noexcept(is_nothrow_piecewise_tuple_constructible<tuple, UTuples&&...>::value)
            : base(piecewise_construct, meta::make_iseq<sizeof...(UTuples)>{},
                   meta::offset_iseq<sizeof...(Ts) - sizeof...(UTuples), sizeof...(UTuples)>{},
                   std::forward<UTuples>(utuples)...)
        {}
        
        // piecewise allocator forwarding constructor
        template<Allocator Alloc, typename... UTuples>
            requires TuplePiecewiseAllocConstructible<tuple, const Alloc&, UTuples&&...> &&
                     TuplePiecewiseAllocConvertible<tuple, const Alloc&, UTuples&&...>
        constexpr
        tuple(piecewise_construct_t, allocator_arg_t, const Alloc& alloc, UTuples&&... utuples)
            noexcept(is_nothrow_piecewise_tuple_alloc_constructible<tuple, const Alloc&,
                                                                    UTuples&&...>::value)
            : base(piecewise_construct, allocator_arg, alloc, meta::make_iseq<sizeof...(UTuples)>{},
                meta::offset_iseq<sizeof...(Ts) - sizeof...(UTuples), sizeof...(UTuples)>{},
                std::forward<UTuples>(utuples)...)
        {}
        
        template<Allocator Alloc, typename... UTuples>
            requires TuplePiecewiseAllocConstructible<tuple, const Alloc&, UTuples&&...> &&
                     !TuplePiecewiseAllocConvertible<tuple, const Alloc&, UTuples&&...>
        explicit constexpr
        tuple(piecewise_construct_t, allocator_arg_t, const Alloc& alloc, UTuples&&... utuples)
            noexcept(is_nothrow_piecewise_tuple_alloc_constructible<tuple, const Alloc&,
                                                                    UTuples&&...>::value)
            : base(piecewise_construct, allocator_arg, alloc, meta::make_iseq<sizeof...(UTuples)>{},
                   meta::offset_iseq<sizeof...(Ts) - sizeof...(UTuples), sizeof...(UTuples)>{},
                   std::forward<UTuples>(utuples)...)
        {}
        
        /////////////////////////////////////////////
        // PERFECT (hopefully) ASSIGNMENT OPERATORS
        /////////////////////////////////////////////
        template<TupleLike UTuple>
            requires TupleTupleAssignable<tuple&, UTuple>
        constexpr tuple& operator=(UTuple&& rhs) &
            noexcept(is_nothrow_tuple_tuple_assignable<tuple&, UTuple>::value)
        { return assign(*this, std::forward<UTuple>(rhs)); }
        
        template<TupleLike UTuple>
            requires TupleTupleAssignable<tuple&&, UTuple>
        constexpr tuple&& operator=(UTuple&& rhs) &&
            noexcept(is_nothrow_tuple_tuple_assignable<tuple&&, UTuple>::value)
        { return assign(std::move(*this), std::forward<UTuple>(rhs)); }
        
        template<TupleLike UTuple>
            requires TupleTupleAssignable<const tuple&, UTuple>
        constexpr const tuple& operator=(UTuple&& rhs) const&
            noexcept(is_nothrow_tuple_tuple_assignable<const tuple&, UTuple>::value)
        { return assign(*this, std::forward<UTuple>(rhs)); }
        
        template<TupleLike UTuple>
            requires TupleTupleAssignable<const tuple&&, UTuple>
        constexpr const tuple&& operator=(UTuple&& rhs) const&&
            noexcept(is_nothrow_tuple_tuple_assignable<const tuple&&, UTuple>::value)
        { return assign(std::move(*this), std::forward<UTuple>(rhs)); }
        
        template<TupleLike UTuple>
            requires TupleTupleAssignable<volatile tuple&, UTuple>
        constexpr volatile tuple& operator=(UTuple&& rhs) volatile&
            noexcept(is_nothrow_tuple_tuple_assignable<volatile tuple&, UTuple>::value)
        { return assign(*this, std::forward<UTuple>(rhs)); }
        
        template<TupleLike UTuple>
            requires TupleTupleAssignable<volatile tuple&&, UTuple>
        constexpr volatile tuple&& operator=(UTuple&& rhs) volatile&&
            noexcept(is_nothrow_tuple_tuple_assignable<volatile tuple&&, UTuple>::value)
        { return assign(std::move(*this), std::forward<UTuple>(rhs)); }
        
        template<TupleLike UTuple>
            requires TupleTupleAssignable<const volatile tuple&, UTuple>
        constexpr const volatile tuple& operator=(UTuple&& rhs) const volatile&
            noexcept(is_nothrow_tuple_tuple_assignable<const volatile tuple&, UTuple>::value)
        { return assign(*this, std::forward<UTuple>(rhs)); }
        
        template<TupleLike UTuple>
            requires TupleTupleAssignable<const volatile tuple&&, UTuple>
        constexpr const volatile tuple&& operator=(UTuple&& rhs) const volatile&&
            noexcept(is_nothrow_tuple_tuple_assignable<const volatile tuple&&, UTuple>::value)
        { return assign(std::move(*this), std::forward<UTuple>(rhs)); }
        
        /////////////////////////////////
        // DELETED ASSIGNMENT OPERATORS
        /////////////////////////////////
        template<TupleLike UTuple>
            requires !TupleTupleAssignable<tuple&, UTuple>
        constexpr tuple& operator=(UTuple&& rhs) & = delete;
        
        template<TupleLike UTuple>
            requires !TupleTupleAssignable<tuple&&, UTuple>
        constexpr tuple&& operator=(UTuple&& rhs) && = delete;
        
        template<TupleLike UTuple>
            requires !TupleTupleAssignable<const tuple&, UTuple>
        constexpr const tuple& operator=(UTuple&& rhs) const& = delete;
        
        template<TupleLike UTuple>
            requires !TupleTupleAssignable<const tuple&&, UTuple>
        constexpr const tuple&& operator=(UTuple&& rhs) const&& = delete;
        
        template<TupleLike UTuple>
            requires !TupleTupleAssignable<volatile tuple&, UTuple>
        constexpr volatile tuple& operator=(UTuple&& rhs) volatile& = delete;
        
        template<TupleLike UTuple>
            requires !TupleTupleAssignable<volatile tuple&&, UTuple>
        constexpr volatile tuple&& operator=(UTuple&& rhs) volatile&& = delete;
        
        template<TupleLike UTuple>
            requires !TupleTupleAssignable<const volatile tuple&, UTuple>
        constexpr const volatile tuple& operator=(UTuple&& rhs) const volatile& = delete;
        
        template<TupleLike UTuple>
            requires !TupleTupleAssignable<const volatile tuple&&, UTuple>
        constexpr const volatile tuple&& operator=(UTuple&& rhs) const volatile&& = delete;
        
        /////////
        // SWAP
        /////////
        template<TupleLike UTuple>
            requires TupleTupleSwappable<tuple&, UTuple&&>
        constexpr void swap(UTuple&& rhs) &
        { ::fcc::swap(*this, std::forward<UTuple>(rhs)); }

        template<TupleLike UTuple>
            requires TupleTupleSwappable<tuple&&, UTuple&&>
        constexpr void swap(UTuple&& rhs) &&
        { ::fcc::swap(std::move(*this), std::forward<UTuple>(rhs)); }
        
        template<TupleLike UTuple>
            requires TupleTupleSwappable<tuple const&, UTuple&&>
        constexpr void swap(UTuple&& rhs) const&
        { ::fcc::swap(*this, std::forward<UTuple>(rhs)); }
        
        template<TupleLike UTuple>
            requires TupleTupleSwappable<tuple const&&, UTuple&&>
        constexpr void swap(UTuple&& rhs) const&&
        { ::fcc::swap(std::move(*this), std::forward<UTuple>(rhs)); }
        
        template<TupleLike UTuple>
            requires TupleTupleSwappable<tuple volatile&, UTuple&&>
        constexpr void swap(UTuple&& rhs) volatile&
        { ::fcc::swap(*this, std::forward<UTuple>(rhs)); }
        
        template<TupleLike UTuple>
            requires TupleTupleSwappable<tuple volatile&&, UTuple&&>
        constexpr void swap(UTuple&& rhs) volatile&&
        { ::fcc::swap(std::move(*this), std::forward<UTuple>(rhs)); }
        
        template<TupleLike UTuple>
            requires TupleTupleSwappable<tuple const volatile&, UTuple&&>
        constexpr void swap(UTuple&& rhs) const volatile&
        { ::fcc::swap(*this, std::forward<UTuple>(rhs)); }
        
        template<TupleLike UTuple>
            requires TupleTupleSwappable<tuple const volatile&&, UTuple&&>
        constexpr void swap(UTuple&& rhs) const volatile&&
        { ::fcc::swap(std::move(*this), std::forward<UTuple>(rhs)); }
        
        /////////////////
        // DELETED SWAP
        /////////////////
        template<TupleLike UTuple>
            requires !TupleTupleSwappable<tuple&, UTuple&&>
        constexpr void swap(UTuple&& rhs) & = delete;

        template<TupleLike UTuple>
            requires !TupleTupleSwappable<tuple&&, UTuple&&>
        constexpr void swap(UTuple&& rhs) && = delete;
        
        template<TupleLike UTuple>
            requires !TupleTupleSwappable<tuple const&, UTuple&&>
        constexpr void swap(UTuple&& rhs) const& = delete;
        
        template<TupleLike UTuple>
            requires !TupleTupleSwappable<tuple const&&, UTuple&&>
        constexpr void swap(UTuple&& rhs) const&& = delete;
        
        template<TupleLike UTuple>
            requires !TupleTupleSwappable<tuple volatile&, UTuple&&>
        constexpr void swap(UTuple&& rhs) volatile& = delete;
        
        template<TupleLike UTuple>
            requires !TupleTupleSwappable<tuple volatile&&, UTuple&&>
        constexpr void swap(UTuple&& rhs) volatile&& = delete;
        
        template<TupleLike UTuple>
            requires !TupleTupleSwappable<tuple const volatile&, UTuple&&>
        constexpr void swap(UTuple&& rhs) const volatile& = delete;
        
        template<TupleLike UTuple>
            requires !TupleTupleSwappable<tuple const volatile&&, UTuple&&>
        constexpr void swap(UTuple&& rhs) const volatile&& = delete;
        
        ///////////////////
        // SIZE - why not
        ///////////////////
        constexpr std::size_t size() const volatile noexcept
        { return sizeof...(Ts); }
        
        ///////////////
        // OPERATOR[]
        ///////////////
        template<std::size_t I> requires (I < sizeof...(Ts))
        constexpr get_type<I, tuple&> operator[](meta::size_t<I>) & noexcept
        { return get_I<I>(*this); }
        
        template<std::size_t I> requires (I < sizeof...(Ts))
        constexpr get_type<I, tuple&&> operator[](meta::size_t<I>) && noexcept
        { return get_I<I>(std::move(*this)); }
        
        template<std::size_t I> requires (I < sizeof...(Ts))
        constexpr get_type<I, const tuple&> operator[](meta::size_t<I>) const& noexcept
        { return get_I<I>(*this); }
        
        template<std::size_t I> requires (I < sizeof...(Ts))
        constexpr get_type<I, const tuple&&> operator[](meta::size_t<I>) const&& noexcept
        { return get_I<I>(std::move(*this)); }
        
        template<std::size_t I> requires (I < sizeof...(Ts))
        constexpr get_type<I, volatile tuple&> operator[](meta::size_t<I>) volatile& noexcept
        { return get_I<I>(*this); }
        
        template<std::size_t I> requires (I < sizeof...(Ts))
        constexpr get_type<I, volatile tuple&&> operator[](meta::size_t<I>) volatile&& noexcept
        { return get_I<I>(std::move(*this)); }
        
        template<std::size_t I> requires (I < sizeof...(Ts))
        constexpr get_type<I, const volatile tuple&>
        operator[](meta::size_t<I>) const volatile& noexcept
        { return get_I<I>(*this); }
        
        template<std::size_t I> requires (I < sizeof...(Ts))
        constexpr get_type<I, const volatile tuple&&>
        operator[](meta::size_t<I>) const volatile&& noexcept
        { return get_I<I>(std::move(*this)); }
    }; // tuple
    
    ///////////////////////
    // get<I>(FccTuple&&)
    ///////////////////////
    namespace detail {
        struct tuple_access {
            template<std::size_t I, FccTuple Tup>
            constexpr get_type<I, Tup> operator()(Tup&& tup) const noexcept
            {
                using elem_type = tuple_element_t<I, std::remove_reference_t<Tup>>;
                using leaf_type = detail::tuple_leaf<I, elem_type>;
                return static_cast<get_type<I, Tup>>(
                    static_cast<meta::as_same_cvref<leaf_type, Tup>>(std::forward<Tup>(tup)).get());
            }
        };
    }
    
    template<std::size_t I, FccTuple Tup> requires (I < size<Tup>::value)
    constexpr get_type<I, Tup> get(Tup&& tup) noexcept
    { return detail::tuple_access{}.template operator()<I>(std::forward<Tup>(tup)); }
}

#endif
