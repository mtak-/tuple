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
#include "test_types.hpp"

#include <memory>
#include <string>
#include <vector>

template<bool assignable, typename Tup, typename T>
struct assign_result_test {};

template<typename Tup, typename T>
struct assign_result_test<true, Tup, T> {
    static_assert(std::is_same<Tup, decltype(std::declval<Tup>() = std::declval<fcc::tuple<T>>())>::value);
};

template<typename Tup, typename T>
static void assign_test()
{
    static_assert(std::is_assignable<Tup, T>::value ==
                  std::is_assignable<meta::as_same_cvref<T, Tup>, fcc::tuple<T>>::value);
    static_assert(std::is_nothrow_assignable<Tup, T>::value ==
                  std::is_nothrow_assignable<meta::as_same_cvref<T, Tup>, fcc::tuple<T>>::value);
    assign_result_test<std::is_assignable<Tup, T>::value, Tup, T>{};
}


template<typename T>
static void assign_tests_impl()
{
    assign_test<fcc::tuple<T>&, T>();
    assign_test<fcc::tuple<T>&&, T>();
    assign_test<const fcc::tuple<T>&, T>();
    assign_test<const fcc::tuple<T>&&, T>();
    assign_test<volatile fcc::tuple<T>&, T>();
    assign_test<volatile fcc::tuple<T>&&, T>();
    assign_test<const volatile fcc::tuple<T>&, T>();
    assign_test<const volatile fcc::tuple<T>&&, T>();
}

template<typename T>
static void assign_tests_ref() {
    assign_tests_impl<T>();
    assign_tests_impl<T&>();
    assign_tests_impl<T&&>();
}

template<typename T>
static void assign_tests_cv() {
    assign_tests_ref<T>();
    assign_tests_ref<const T>();
    assign_tests_ref<volatile T>();
    assign_tests_ref<const volatile T>();
}

static void assign_tests() {
    assign_tests_cv<int>();
    assign_tests_cv<std::string>();
}

int main() {
    assign_tests();
    return test_result();
}