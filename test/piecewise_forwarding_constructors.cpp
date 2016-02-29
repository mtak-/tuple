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

struct my_alloc : std::allocator<int>
{
    template<typename T>
    struct rebind { using other = my_alloc; };
    
    my_alloc() noexcept(false) : std::allocator<int>() {}
    my_alloc(const my_alloc&) noexcept(false) : std::allocator<int>() {}
    my_alloc(my_alloc&&) noexcept(false) : std::allocator<int>() {}
    my_alloc& operator=(const my_alloc&) noexcept(false) { return *this; }
    my_alloc& operator=(my_alloc&&) noexcept(false) { return *this; }
};

int main() {
    {
        CHECK(noexcept(fcc::tuple<>{fcc::piecewise_construct}));
        CHECK(noexcept(fcc::tuple<>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}}));
        CHECK(noexcept(fcc::tuple<fcc::tuple<>>{fcc::piecewise_construct}));
        CHECK(noexcept(fcc::tuple<fcc::tuple<>>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}}));
        CHECK(noexcept(fcc::tuple<fcc::tuple<>>{fcc::piecewise_construct, fcc::tuple<>{}}));
        CHECK(noexcept(fcc::tuple<fcc::tuple<>>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}, fcc::tuple<>{}}));
    }

    struct A {
        int i = 44;
        
        A() noexcept {}
        A(const A&) = delete;
        A& operator=(const A&) = delete;
    };
    {
        NOEXCEPT_CHECK(fcc::tuple<A>{fcc::piecewise_construct}[0_c].i == 44);
        NOEXCEPT_CHECK(fcc::tuple<A>{fcc::piecewise_construct, fcc::tuple<>{}}[0_c].i == 44);
        NOEXCEPT_CHECK(fcc::tuple<A>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}}[0_c].i == 44);
        NOEXCEPT_CHECK(fcc::tuple<A>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}, fcc::tuple<>{}}[0_c].i == 44);
    }
    
    struct B {
        int i;
        std::string s;
        
        B(int i_in, std::string s_in) : i(i_in), s(std::move(s_in)) {}
        B(const B&) = delete;
        B& operator=(const B&) = delete;
    };
    {
        EXCEPT_CHECK(fcc::tuple<B>{fcc::piecewise_construct, fcc::tuple<int, const char*>{{42}, "hello"}}[0_c].i == 42);
        EXCEPT_CHECK(fcc::tuple<B>{fcc::piecewise_construct, fcc::tuple<int, const char*>{{42}, "hello"}}[0_c].s == "hello");
        EXCEPT_CHECK(fcc::tuple<B>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}, fcc::tuple<int, const char*>{{42}, "hello"}}[0_c].i == 42);
        EXCEPT_CHECK(fcc::tuple<B>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}, fcc::tuple<int, const char*>{{42}, "hello"}}[0_c].s == "hello");
    }
    
    struct C {
        int i;
        double d;
        
        C(int i_in, double d_in) noexcept : i(i_in), d(d_in) {}
        C(const C&) = delete;
        C& operator=(const C&) = delete;
    };
    {
        NOEXCEPT_CHECK(fcc::tuple<C>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}}[0_c].i == 42);
        NOEXCEPT_CHECK(fcc::tuple<C>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}}[0_c].d == 42.8);
        NOEXCEPT_CHECK(fcc::tuple<C>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}, fcc::tuple<int, double>{42, 42.8}}[0_c].i == 42);
        NOEXCEPT_CHECK(fcc::tuple<C>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}, fcc::tuple<int, double>{42, 42.8}}[0_c].d == 42.8);
    }
    
    {
        EXCEPT_CHECK(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}, fcc::tuple<>{}}[0_c].i == 42);
        EXCEPT_CHECK(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}, fcc::tuple<>{}}[0_c].d == 42.8);
        EXCEPT_CHECK(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}, fcc::tuple<>{}}[1_c].i == 43);
        EXCEPT_CHECK(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}, fcc::tuple<>{}}[1_c].s == "hello");
        EXCEPT_CHECK(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}, fcc::tuple<>{}}[2_c].i == 44);
    }
    
    {
        EXCEPT_CHECK(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}}[0_c].i == 42);
        EXCEPT_CHECK(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}}[0_c].d == 42.8);
        EXCEPT_CHECK(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}}[1_c].i == 43);
        EXCEPT_CHECK(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}}[1_c].s == "hello");
        EXCEPT_CHECK(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}}[2_c].i == 44);
    }
    
    // nesting check
    {
        EXCEPT_CHECK(fcc::tuple<fcc::tuple<C, B, A>>{fcc::piecewise_construct, fcc::make_tuple(fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"})}[0_c][0_c].i == 42);
        EXCEPT_CHECK(fcc::tuple<fcc::tuple<C, B, A>>{fcc::piecewise_construct, fcc::make_tuple(fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"})}[0_c][0_c].d == 42.8);
        EXCEPT_CHECK(fcc::tuple<fcc::tuple<C, B, A>>{fcc::piecewise_construct, fcc::make_tuple(fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"})}[0_c][1_c].i == 43);
        EXCEPT_CHECK(fcc::tuple<fcc::tuple<C, B, A>>{fcc::piecewise_construct, fcc::make_tuple(fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"})}[0_c][1_c].s == "hello");
        EXCEPT_CHECK(fcc::tuple<fcc::tuple<C, B, A>>{fcc::piecewise_construct, fcc::make_tuple(fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"})}[0_c][2_c].i == 44);
    }
    
    // actually use allocator check
    {
        CHECK(noexcept(fcc::tuple<std::vector<int>>{fcc::piecewise_construct}));
        CHECK(noexcept(fcc::tuple<std::vector<int>>{fcc::piecewise_construct, fcc::make_tuple()}));
        CHECK(noexcept(fcc::tuple<std::vector<int>>{fcc::piecewise_construct, fcc::make_tuple(std::allocator<int>{})}));
        CHECK(!noexcept(fcc::tuple<std::vector<int, my_alloc>>{fcc::piecewise_construct}));
        CHECK(!noexcept(fcc::tuple<std::vector<int, my_alloc>>{fcc::piecewise_construct, fcc::make_tuple()}));
        
        CHECK(noexcept(fcc::tuple<std::vector<int>>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}}));
        CHECK(noexcept(fcc::tuple<std::vector<int>>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}, fcc::make_tuple()}));
        
        my_alloc alloc;
        static_assert(noexcept(my_alloc()) == false);
        
        // this should fail in c++17
        // when it does, add back in the tests below
        static_assert(std::is_nothrow_constructible<std::vector<int, my_alloc>, const my_alloc&>::value);
        
//        CHECK(!noexcept(fcc::tuple<std::vector<int, my_alloc>>{fcc::piecewise_construct, fcc::allocator_arg, alloc}));
//        CHECK(!noexcept(fcc::tuple<std::vector<int, my_alloc>>{fcc::piecewise_construct, fcc::allocator_arg, alloc, fcc::make_tuple()}));
    }
    
    return test_result();
}
