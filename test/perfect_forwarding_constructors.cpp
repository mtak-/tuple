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

int main() {
    struct A { explicit A(int i) noexcept : x(i) {} int x; };
    static_assert(fcc::is_convertible<A, int&&>::value == false);
    static_assert(std::is_constructible<fcc::tuple<A>, int&&>::value == true);
    static_assert(fcc::is_convertible<fcc::tuple<A>, int&&>::value == false);
    
    struct B { B(int i) noexcept : x(i) {} int x; };
    static_assert(fcc::is_convertible<B, int&&>::value == true);
    static_assert(std::is_constructible<fcc::tuple<B>, int&&>::value == true);
    static_assert(fcc::is_convertible<fcc::tuple<B>, int&&>::value == true);
    
    static_assert(std::is_constructible<fcc::tuple<A, B>, int&&, int&&>::value == true);
    static_assert(fcc::is_convertible<fcc::tuple<A, B>, int, int>::value == false);
    
    static_assert(std::is_constructible<fcc::tuple<B, A>, int&&, int&&>::value == true);
    static_assert(fcc::is_convertible<fcc::tuple<B, A>, int, int>::value == false);
    
    NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<A, B>{A(2), 3}).x == 2);
    NOEXCEPT_CHECK(fcc::get<1>(fcc::tuple<A, B>{A(2), 3}).x == 3);
    NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<B, A>{2, A(3)}).x == 2);
    NOEXCEPT_CHECK(fcc::get<1>(fcc::tuple<B, A>{2, A(3)}).x == 3);
    
    {
        struct C { explicit C() {} };
        struct D { D() {} };
        static_assert(std::is_constructible<fcc::tuple<C, D>, C>::value);
        static_assert(fcc::is_convertible<fcc::tuple<C, D>, C>::value);
        static_assert(std::is_constructible<fcc::tuple<D, C>, D>::value);
        static_assert(fcc::is_convertible<fcc::tuple<D, C>, D>::value == false);
    }
    
    {
        auto ptr = new int(4);
        NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<std::unique_ptr<int>>{ptr}).get() == ptr);
        EXCEPT_CHECK(*fcc::get<0>(fcc::tuple<std::unique_ptr<int>>{std::unique_ptr<int>{new int(5)}}) == 5);
    }
    
    {
        static_assert(fcc::is_convertible<fcc::tuple<std::vector<int>>, fcc::allocator_arg_t, std::allocator<int>, std::initializer_list<int>>::value);
        static_assert(fcc::is_convertible<fcc::tuple<std::vector<int>>, fcc::allocator_arg_t, std::allocator<int>, std::size_t>::value == false);
        static_assert(std::is_constructible<fcc::tuple<std::vector<int>>, fcc::allocator_arg_t, std::allocator<int>, std::size_t>::value == true);
        fcc::tuple<std::vector<int>> x = {fcc::allocator_arg, std::allocator<int>{}, std::initializer_list<int>{42}};
        CHECK(fcc::get<0>(x)[0] == 42);
        CHECK(fcc::get<0>(x).size() == 1u);
        CHECK(!noexcept(fcc::tuple<std::vector<int>>{fcc::allocator_arg, std::allocator<int>{}, std::initializer_list<int>{42}}));
        CHECK(!noexcept(fcc::tuple<std::vector<int>, int>{fcc::allocator_arg, std::allocator<int>{}, std::initializer_list<int>{42}}));
        EXCEPT_CHECK(fcc::get<1>(fcc::tuple<std::vector<int>, int>{fcc::allocator_arg, std::allocator<int>{}, std::initializer_list<int>{42}, 43}) == 43);
        CHECK(fcc::get<0>(fcc::tuple<int, std::vector<int>>{fcc::allocator_arg, std::allocator<int>{}, 43}) == 43);
        CHECK(noexcept(fcc::tuple<int, std::vector<int>>{fcc::allocator_arg, std::allocator<int>{}, 43}) == noexcept(std::vector<int>{std::allocator<int>{}}));
        EXCEPT_CHECK(fcc::get<0>(fcc::tuple<int, std::vector<int>>{fcc::allocator_arg, std::allocator<int>{}, 43, std::initializer_list<int>{42}}) == 43);
    }
    
    {
        volatile volatile_rref_constructor i;
        NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<volatile_rref_constructor>{}).i == 42);
        NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<volatile_rref_constructor>{std::move(i)}).i == 43);
        NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<volatile_rref_constructor>{fcc::allocator_arg, std::allocator<int>{}, std::move(i)}).i == 43);
    }

    return test_result();
}