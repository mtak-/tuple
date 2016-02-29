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
        NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<A>{fcc::piecewise_construct}).i == 44);
        NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<A>{fcc::piecewise_construct, fcc::tuple<>{}}).i == 44);
        NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<A>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}}).i == 44);
        NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<A>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}, fcc::tuple<>{}}).i == 44);
    }
    
    struct B {
        int i;
        std::string s;
        
        B(int i_in, std::string s_in) : i(i_in), s(std::move(s_in)) {}
        B(const B&) = delete;
        B& operator=(const B&) = delete;
    };
    {
        EXCEPT_CHECK(fcc::get<0>(fcc::tuple<B>{fcc::piecewise_construct, fcc::tuple<int, const char*>{{42}, "hello"}}).i == 42);
        EXCEPT_CHECK(fcc::get<0>(fcc::tuple<B>{fcc::piecewise_construct, fcc::tuple<int, const char*>{{42}, "hello"}}).s == "hello");
        EXCEPT_CHECK(fcc::get<0>(fcc::tuple<B>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}, fcc::tuple<int, const char*>{{42}, "hello"}}).i == 42);
        EXCEPT_CHECK(fcc::get<0>(fcc::tuple<B>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}, fcc::tuple<int, const char*>{{42}, "hello"}}).s == "hello");
    }
    
    struct C {
        int i;
        double d;
        
        C(int i_in, double d_in) noexcept : i(i_in), d(d_in) {}
        C(const C&) = delete;
        C& operator=(const C&) = delete;
    };
    {
        NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<C>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}}).i == 42);
        NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<C>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}}).d == 42.8);
        NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<C>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}, fcc::tuple<int, double>{42, 42.8}}).i == 42);
        NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<C>{fcc::piecewise_construct, fcc::allocator_arg, std::allocator<int>{}, fcc::tuple<int, double>{42, 42.8}}).d == 42.8);
    }
    
    {
        EXCEPT_CHECK(fcc::get<0>(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}, fcc::tuple<>{}}).i == 42);
        EXCEPT_CHECK(fcc::get<0>(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}, fcc::tuple<>{}}).d == 42.8);
        EXCEPT_CHECK(fcc::get<1>(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}, fcc::tuple<>{}}).i == 43);
        EXCEPT_CHECK(fcc::get<1>(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}, fcc::tuple<>{}}).s == "hello");
        EXCEPT_CHECK(fcc::get<2>(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}, fcc::tuple<>{}}).i == 44);
    }
    
    {
        EXCEPT_CHECK(fcc::get<0>(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}}).i == 42);
        EXCEPT_CHECK(fcc::get<0>(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}}).d == 42.8);
        EXCEPT_CHECK(fcc::get<1>(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}}).i == 43);
        EXCEPT_CHECK(fcc::get<1>(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}}).s == "hello");
        EXCEPT_CHECK(fcc::get<2>(fcc::tuple<C, B, A>{fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"}}).i == 44);
    }
    
    // nesting check
    {
        EXCEPT_CHECK(fcc::get<0>(fcc::get<0>(fcc::tuple<fcc::tuple<C, B, A>>{fcc::piecewise_construct, fcc::make_tuple(fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"})})).i == 42);
        EXCEPT_CHECK(fcc::get<0>(fcc::get<0>(fcc::tuple<fcc::tuple<C, B, A>>{fcc::piecewise_construct, fcc::make_tuple(fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"})})).d == 42.8);
        EXCEPT_CHECK(fcc::get<1>(fcc::get<0>(fcc::tuple<fcc::tuple<C, B, A>>{fcc::piecewise_construct, fcc::make_tuple(fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"})})).i == 43);
        EXCEPT_CHECK(fcc::get<1>(fcc::get<0>(fcc::tuple<fcc::tuple<C, B, A>>{fcc::piecewise_construct, fcc::make_tuple(fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"})})).s == "hello");
        EXCEPT_CHECK(fcc::get<2>(fcc::get<0>(fcc::tuple<fcc::tuple<C, B, A>>{fcc::piecewise_construct, fcc::make_tuple(fcc::piecewise_construct, fcc::tuple<int, double>{42, 42.8}, fcc::tuple<int, const char*>{43, "hello"})})).i == 44);
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
