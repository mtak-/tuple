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

using namespace fcc::literals;

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
    
    NOEXCEPT_CHECK(fcc::tuple<A, B>{A(2), 3}[0_c].x == 2);
    NOEXCEPT_CHECK(fcc::tuple<A, B>{A(2), 3}[1_c].x == 3);
    NOEXCEPT_CHECK(fcc::tuple<B, A>{2, A(3)}[0_c].x == 2);
    NOEXCEPT_CHECK(fcc::tuple<B, A>{2, A(3)}[1_c].x == 3);
    
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
        NOEXCEPT_CHECK(fcc::tuple<std::unique_ptr<int>>{ptr}[0_c].get() == ptr);
        EXCEPT_CHECK(*fcc::tuple<std::unique_ptr<int>>{std::unique_ptr<int>{new int(5)}}[0_c] == 5);
    }
    
    {
        static_assert(fcc::is_convertible<fcc::tuple<std::vector<int>>, fcc::allocator_arg_t, std::allocator<int>, std::initializer_list<int>>::value);
        static_assert(fcc::is_convertible<fcc::tuple<std::vector<int>>, fcc::allocator_arg_t, std::allocator<int>, std::size_t>::value == false);
        static_assert(std::is_constructible<fcc::tuple<std::vector<int>>, fcc::allocator_arg_t, std::allocator<int>, std::size_t>::value == true);
        fcc::tuple<std::vector<int>> x = {fcc::allocator_arg, std::allocator<int>{}, std::initializer_list<int>{42}};
        CHECK(x[0_c][0_c] == 42);
        CHECK(x[0_c].size() == 1u);
        CHECK(!noexcept(fcc::tuple<std::vector<int>>{fcc::allocator_arg, std::allocator<int>{}, std::initializer_list<int>{42}}));
        CHECK(!noexcept(fcc::tuple<std::vector<int>, int>{fcc::allocator_arg, std::allocator<int>{}, std::initializer_list<int>{42}}));
        EXCEPT_CHECK(fcc::tuple<std::vector<int>, int>{fcc::allocator_arg, std::allocator<int>{}, std::initializer_list<int>{42}, 43}[1_c] == 43);
        CHECK(fcc::tuple<int, std::vector<int>>{fcc::allocator_arg, std::allocator<int>{}, 43}[0_c] == 43);
        CHECK(noexcept(fcc::tuple<int, std::vector<int>>{fcc::allocator_arg, std::allocator<int>{}, 43}) == noexcept(std::vector<int>{std::allocator<int>{}}));
        EXCEPT_CHECK(fcc::tuple<int, std::vector<int>>{fcc::allocator_arg, std::allocator<int>{}, 43, std::initializer_list<int>{42}}[0_c] == 43);
    }
    
    {
        volatile volatile_rref_constructor i;
        NOEXCEPT_CHECK(fcc::tuple<volatile_rref_constructor>{}[0_c].i == 42);
        NOEXCEPT_CHECK(fcc::tuple<volatile_rref_constructor>{std::move(i)}[0_c].i == 43);
        NOEXCEPT_CHECK(fcc::tuple<volatile_rref_constructor>{fcc::allocator_arg, std::allocator<int>{}, std::move(i)}[0_c].i == 43);
    }

    return test_result();
}