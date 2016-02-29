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

#include "simple_test.hpp"

#include <string>

using namespace fcc;

template<typename T>
static void index_operator_test()
{
    static_assert(
        std::is_same<
            T&,
            decltype(std::declval<tuple<T>&>()[0_c])
        >::value);
    
    static_assert(noexcept(std::declval<tuple<T>&>()[0_c]));
    
    static_assert(
        std::is_same<
            T&&,
            decltype(std::declval<tuple<T>&&>()[0_c])
        >::value);
    
    static_assert(noexcept(std::declval<tuple<T>&&>()[0_c]));
    
    static_assert(
        std::is_same<
            const T&,
            decltype(std::declval<const tuple<T>&>()[0_c])
        >::value);
    
    static_assert(noexcept(std::declval<const tuple<T>&>()[0_c]));
    
    static_assert(
        std::is_same<
            const T&&,
            decltype(std::declval<const tuple<T>&&>()[0_c])
        >::value);
    
    static_assert(noexcept(std::declval<const tuple<T>&&>()[0_c]));
    
    static_assert(
        std::is_same<
            volatile T&,
            decltype(std::declval<volatile tuple<T>&>()[0_c])
        >::value);
    
    static_assert(noexcept(std::declval<volatile tuple<T>&>()[0_c]));
    
    static_assert(
        std::is_same<
            volatile T&&,
            decltype(std::declval<volatile tuple<T>&&>()[0_c])
        >::value);
    
    static_assert(noexcept(std::declval<volatile tuple<T>&&>()[0_c]));
    
    static_assert(
        std::is_same<
            const volatile T&,
            decltype(std::declval<const volatile tuple<T>&>()[0_c])
        >::value);
    
    static_assert(noexcept(std::declval<const volatile tuple<T>&>()[0_c]));
    
    static_assert(
        std::is_same<
            const volatile T&&,
            decltype(std::declval<const volatile tuple<T>&&>()[0_c])
        >::value);
    
    static_assert(noexcept(std::declval<const volatile tuple<T>&&>()[0_c]));
}

// static_assert get_type is the same as the type std::get returns
template<typename T>
static void get_tests_impl()
{
    index_operator_test<T>();
    
    static_assert(
        std::is_same<
            T&,
            decltype(get<0>(std::declval<tuple<T>&>()))
        >::value);
    
    static_assert(noexcept(get<0>(std::declval<tuple<T>&>())));
    
    static_assert(
        std::is_same<
            T&&,
            decltype(get<0>(std::declval<tuple<T>&&>()))
        >::value);
    
    static_assert(noexcept(get<0>(std::declval<tuple<T>&&>())));
    
    static_assert(
        std::is_same<
            const T&,
            decltype(get<0>(std::declval<const tuple<T>&>()))
        >::value);
    
    static_assert(noexcept(get<0>(std::declval<const tuple<T>&>())));
    
    static_assert(
        std::is_same<
            const T&&,
            decltype(get<0>(std::declval<const tuple<T>&&>()))
        >::value);
    
    static_assert(noexcept(get<0>(std::declval<const tuple<T>&&>())));
    
    static_assert(
        std::is_same<
            volatile T&,
            decltype(get<0>(std::declval<volatile tuple<T>&>()))
        >::value);
    
    static_assert(noexcept(get<0>(std::declval<volatile tuple<T>&>())));
    
    static_assert(
        std::is_same<
            volatile T&&,
            decltype(get<0>(std::declval<volatile tuple<T>&&>()))
        >::value);
    
    static_assert(noexcept(get<0>(std::declval<volatile tuple<T>&&>())));
    
    static_assert(
        std::is_same<
            const volatile T&,
            decltype(get<0>(std::declval<const volatile tuple<T>&>()))
        >::value);
    
    static_assert(noexcept(get<0>(std::declval<const volatile tuple<T>&>())));
    
    static_assert(
        std::is_same<
            const volatile T&&,
            decltype(get<0>(std::declval<const volatile tuple<T>&&>()))
        >::value);
    
    static_assert(noexcept(get<0>(std::declval<const volatile tuple<T>&&>())));
}

template<typename T>
static void get_tests_ref() {
    get_tests_impl<T>();
    get_tests_impl<T&>();
    get_tests_impl<T&&>();
}

template<typename T>
static void get_tests_cv() {
    get_tests_ref<T>();
    get_tests_ref<const T>();
    get_tests_ref<volatile T>();
    get_tests_ref<const volatile T>();
}

static void get_tests() {
    get_tests_cv<int>();
    get_tests_cv<std::string>();
}

int main()
{
    get_tests();
    return test_result();
}