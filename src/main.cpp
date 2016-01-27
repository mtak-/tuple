//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#include <fcc/tuple.hpp>

#include "../test/test_types.hpp"
#include "config.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <type_traits>

// note: the tests try all combinations, but do not print out any information on
// volatile tuples. though they still print out tuples containing volatile types
static_assert(std::is_same<meta::uncvref<TYPE>, TYPE>{},
    "Do not add const/volatile or references to TYPE");

// give lvalue_ref to T
template<typename T, typename U>
using lval_assignable = std::is_assignable<T&, U>;

// give rvalue_ref to T
template<typename T, typename U>
using rval_assignable = std::is_assignable<T&&, U>;

#define LOG(t, u)\
    do {\
        NO_CONSTRUCTION(log<t, u, std::is_constructible>(#t, #u, "is_constructible"));\
        NO_LVAL_ASSIGNMENT(log<t, u, lval_assignable>(#t, #u, "lval_assignable"));\
        NO_RVAL_ASSIGNMENT(log<t, u, rval_assignable>(#t, #u, "rval_assignable"));\
    } while(0)

#define LOG_COMBOS(t, u) LOG_COMBOS_I(t, u)
#define LOG_COMBOS_I(t, u) \
    std::cout << "  lhs: " #t << std::endl; \
    LOG(t, u); \
    LOG(t, const u); \
    LOG(t, volatile u); \
    LOG(t, const volatile u); \
    std::cout << std::endl << "  lhs: const " #t << std::endl; \
    LOG(const t, u); \
    LOG(const t, const u); \
    LOG(const t, volatile u); \
    LOG(const t, const volatile u); \
    std::cout << std::endl << "  lhs: volatile " #t << std::endl; \
    LOG(volatile t, u); \
    LOG(volatile t, const u); \
    LOG(volatile t, volatile u); \
    LOG(volatile t, const volatile u); \
    std::cout << std::endl << "  lhs: const volatile " #t << std::endl; \
    LOG(const volatile t, u); \
    LOG(const volatile t, const u); \
    LOG(const volatile t, volatile u); \
    LOG(const volatile t, const volatile u); \
    std::cout << std::endl;

#define TYPE_SIZE(t) TYPE_SIZE_I(t)
#define TYPE_SIZE_I(t) size(#t)

template<std::size_t I>
static constexpr int size(const char(&)[I])
{
    return static_cast<int>(I);
}

template<template<typename...> class>
struct type_trait_is_assignable : std::false_type {};

template<>
struct type_trait_is_assignable<lval_assignable> : std::true_type {};

template<>
struct type_trait_is_assignable<rval_assignable> : std::true_type {};

template<template<typename...> class type_trait, typename Lhs, typename Rhs, std::size_t... Is>
static void verify_via_static_assert_impl(meta::iseq<Is...>)
{
    // both constructibility and assignibility use get_type for Rhs
    static_assert(
        // constructibility uses tuple_element_t for Lhs
        (!type_trait_is_assignable<type_trait>{}() &&
        meta::and_c<
            type_trait<Lhs, Rhs>{}() == type_trait<fcc::tuple_element_t<Is, Lhs>,
                                                   fcc::get_type<Is, Rhs>>{}()...>{}) ||
        // assignability uses get_type for Lhs
        (type_trait_is_assignable<type_trait>{}() &&
        meta::and_c<
            type_trait<Lhs, Rhs>{}() == type_trait<fcc::get_type<Is, Lhs>,
                                                   fcc::get_type<Is, Rhs>>{}()...>{}),
        ""
    );
}

template<template<typename...> class type_trait, typename Lhs, typename Rhs>
static void verify_via_static_assert()
{
// std::tuple fails these tests
#if USE_STD_TUPLE == 0
    verify_via_static_assert_impl<type_trait, Lhs, Rhs>(fcc::tuple_indices<Lhs>{});
#endif
}

static void log_out_impl(int width, bool colored, bool evaluates_to, const char* T_name, const char* U_name,
    const char* trait_name, const char* prefix_text, const char* middle_text, const char* end_text)
{
    std::string colored_text = "\033[0;0m";
    if (colored)
        colored_text = "\033[1;31m";
    std::cout
        << colored_text
        << "  "
        << std::setw(width)
        << (std::string(trait_name)
            + "<"
            + prefix_text
            + T_name
            + middle_text
            + U_name
            + end_text
            + ">: ")
        << (evaluates_to ? 'T' : 'F')
        << std::endl;
    std::cout << "\033[0;0m";
}

template<typename T, typename U, template<typename...> class type_trait>
static void log(const char* T_name, const char* U_name, const char* trait_name)
{
    static constexpr auto width = 90 + TYPE_SIZE(TYPE) * 2;
    static constexpr auto other_width = 1;
    
    constexpr bool traitable =
        type_trait<T, U>::value;
    
    constexpr bool tuple_traitable_u =
        type_trait<TUPLE_TYPE<T>, U>{}();
    
    
    constexpr bool tuple_traitable_rref =
        type_trait<TUPLE_TYPE<T>, TUPLE_TYPE<U>>{}();
    verify_via_static_assert<type_trait, TUPLE_TYPE<T>, TUPLE_TYPE<U>>();
    
    constexpr bool tuple_traitable_crref =
        type_trait<TUPLE_TYPE<T>, const TUPLE_TYPE<U>>{}();
    verify_via_static_assert<type_trait, TUPLE_TYPE<T>, const TUPLE_TYPE<U>>();
    
    constexpr bool tuple_traitable_vrref =
        type_trait<TUPLE_TYPE<T>, volatile TUPLE_TYPE<U>>{}();
    verify_via_static_assert<type_trait, TUPLE_TYPE<T>, volatile TUPLE_TYPE<U>>();
    
    constexpr bool tuple_traitable_cvrref =
        type_trait<TUPLE_TYPE<T>, const volatile TUPLE_TYPE<U>>{}();
    verify_via_static_assert<type_trait, TUPLE_TYPE<T>, const volatile TUPLE_TYPE<U>>();
    
    
    constexpr bool tuple_traitable_lref =
        type_trait<TUPLE_TYPE<T>, TUPLE_TYPE<U>&>{}();
    verify_via_static_assert<type_trait, TUPLE_TYPE<T>, TUPLE_TYPE<U>&>();
    
    constexpr bool tuple_traitable_clref =
        type_trait<TUPLE_TYPE<T>, const TUPLE_TYPE<U>&>{}();
    verify_via_static_assert<type_trait, TUPLE_TYPE<T>, const TUPLE_TYPE<U>&>();
    
    constexpr bool tuple_traitable_vlref =
        type_trait<TUPLE_TYPE<T>, volatile TUPLE_TYPE<U>&>{}();
    verify_via_static_assert<type_trait, TUPLE_TYPE<T>, volatile TUPLE_TYPE<U>&>();
    
    constexpr bool tuple_traitable_cvlref =
        type_trait<TUPLE_TYPE<T>, const volatile TUPLE_TYPE<U>&>{}();
    verify_via_static_assert<type_trait, TUPLE_TYPE<T>, const volatile TUPLE_TYPE<U>&>();
    
    // attempt at highlighting interesting bits
    constexpr bool same =
        (traitable == tuple_traitable_u || type_trait_is_assignable<type_trait>{}) &&
        traitable == tuple_traitable_clref &&
        traitable == tuple_traitable_rref;
    
    auto log_out = [=](bool evaluates_to, const char* prefix_text, const char* middle_text, const char* end_text)
    {
        log_out_impl(width, !same, evaluates_to, T_name, U_name, trait_name, prefix_text, middle_text, end_text);
    };
    
    std::string colored_text = "\033[0;0m";
    if (!same)
        colored_text = "\033[1;31m";
    
    auto u_name_length = static_cast<int>(std::string(U_name).size());
    auto half_width = (width + other_width - u_name_length) / 2 - 3;
    std::cout
        << colored_text
        << "  "
        << std::setw(half_width)
        << std::setfill('-')
        << ""
        << " rhs: "
        << U_name
        << " "
        << std::setw(width + other_width - half_width - u_name_length - 7)
        << ""
        << std::endl
        << std::setfill(' ');
    log_out(traitable, "",/*type*/ ", ",/*type*/ "");
    if (!type_trait_is_assignable<type_trait>{})
        log_out(tuple_traitable_u, "tuple<",/*type*/ ">, ",/*type*/ "");
    log_out(tuple_traitable_rref, "tuple<",/*type*/ ">, tuple<",/*type*/ ">&&");
    log_out(tuple_traitable_clref, "tuple<",/*type*/ ">, const tuple<",/*type*/ ">&");
}

extern void playground();

int main()
{
    using std::cout;
    using std::endl;
    
    // prvalues
    {
        cout << "prvalues from prvalues" << endl;
        LOG_COMBOS(TYPE, TYPE);
        cout << endl;
        
        cout << "prvalues from lrefs" << endl;
        LOG_COMBOS(TYPE, TYPE&);
        cout << endl;
        
        cout << "prvalues from rrefs" << endl;
        LOG_COMBOS(TYPE, TYPE&&);
        cout << endl;
    }
    // lrefs
    {
        cout << "lrefs from prvalues" << endl;
        LOG_COMBOS(TYPE&, TYPE);
        cout << endl;
        
        cout << "lrefs from lrefs" << endl;
        LOG_COMBOS(TYPE&, TYPE&);
        cout << endl;
        
        cout << "lrefs from rrefs" << endl;
        LOG_COMBOS(TYPE&, TYPE&&);
        cout << endl;
    }
    // rrefs
    {
        cout << "rrefs from prvalues" << endl;
        LOG_COMBOS(TYPE&&, TYPE);
        cout << endl;
        
        cout << "rrefs from lrefs" << endl;
        LOG_COMBOS(TYPE&&, TYPE&);
        cout << endl;
        
        cout << "rrefs from rrefs" << endl;
        LOG_COMBOS(TYPE&&, TYPE&&);
        cout << endl;
    }
    
    // verify type traits still work
    //  even though these member functions are marked as deleted, they don't participate
    //  in overload resolution (requires false)
    static_assert(
        std::is_copy_constructible<TUPLE_TYPE<TYPE>>{} == std::is_copy_constructible<TYPE>{}
        , "");
    static_assert(
        std::is_move_constructible<TUPLE_TYPE<TYPE>>{} == std::is_move_constructible<TYPE>{}
        , "");
    static_assert(
        std::is_copy_assignable<TUPLE_TYPE<TYPE>>{} == std::is_copy_assignable<TYPE>{}
        , "");
    static_assert(
        std::is_move_assignable<TUPLE_TYPE<TYPE>>{} == std::is_move_assignable<TYPE>{}
        , "");
    
    playground();
    
    return 0;
}
