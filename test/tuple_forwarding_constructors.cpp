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
    {
        volatile fcc::tuple<volatile_rref_constructor> x;
        NOEXCEPT_CHECK(fcc::tuple<volatile_rref_constructor>{}[0_c].i == 42);
        NOEXCEPT_CHECK(fcc::tuple<volatile_rref_constructor>{std::move(x)}[0_c].i == 43);
        fcc::tuple<volatile volatile_rref_constructor> z{std::move(x)};
        NOEXCEPT_CHECK(fcc::tuple<volatile volatile_rref_constructor>{std::move(x)}[0_c].i == 43);
        NOEXCEPT_CHECK(fcc::tuple<volatile_rref_constructor>{fcc::allocator_arg, std::allocator<int>{}, std::move(z)}[0_c].i == 43);
        NOEXCEPT_CHECK(fcc::tuple<volatile_rref_constructor>{fcc::allocator_arg, std::allocator<int>{}, std::move(x)}[0_c].i == 43);
    }
    
    {
        CHECK(noexcept(fcc::tuple<>{fcc::tuple<>{}}));
        
        volatile fcc::tuple<> x{};
        CHECK(noexcept(fcc::tuple<>{x}));
        CHECK(noexcept(fcc::tuple<>{std::move(x)}));
        CHECK(noexcept(fcc::tuple<>{fcc::allocator_arg, std::allocator<int>{}, x}));
        CHECK(noexcept(fcc::tuple<>{fcc::allocator_arg, std::allocator<int>{}, std::move(x)}));
        
        const volatile fcc::tuple<> y{};
        CHECK(noexcept(fcc::tuple<>{y}));
        CHECK(noexcept(fcc::tuple<>{std::move(y)}));
        CHECK(noexcept(fcc::tuple<>{fcc::allocator_arg, std::allocator<int>{}, y}));
        CHECK(noexcept(fcc::tuple<>{fcc::allocator_arg, std::allocator<int>{}, std::move(y)}));
    }
    
    {
        fcc::tuple<int, double, float> x{42, 42.5, 42.8f};
        NOEXCEPT_CHECK(fcc::tuple<int, double, float>{x}[0_c] == 42);
        NOEXCEPT_CHECK(fcc::tuple<int, double, float>{x}[1_c] == 42.5);
        NOEXCEPT_CHECK(fcc::tuple<int, double, float>{x}[2_c] == 42.8f);
        NOEXCEPT_CHECK(fcc::tuple<int, double, float>{std::move(x)}[0_c] == 42);
        NOEXCEPT_CHECK(fcc::tuple<int, double, float>{std::move(x)}[1_c] == 42.5);
        NOEXCEPT_CHECK(fcc::tuple<int, double, float>{std::move(x)}[2_c] == 42.8f);
        NOEXCEPT_CHECK(fcc::tuple<int, double, float>{fcc::allocator_arg, std::allocator<int>{}, x}[0_c] == 42);
        NOEXCEPT_CHECK(fcc::tuple<int, double, float>{fcc::allocator_arg, std::allocator<int>{}, x}[1_c] == 42.5);
        NOEXCEPT_CHECK(fcc::tuple<int, double, float>{fcc::allocator_arg, std::allocator<int>{}, x}[2_c] == 42.8f);
        NOEXCEPT_CHECK(fcc::tuple<int, double, float>{fcc::allocator_arg, std::allocator<int>{}, std::move(x)}[0_c] == 42);
        NOEXCEPT_CHECK(fcc::tuple<int, double, float>{fcc::allocator_arg, std::allocator<int>{}, std::move(x)}[1_c] == 42.5);
        NOEXCEPT_CHECK(fcc::tuple<int, double, float>{fcc::allocator_arg, std::allocator<int>{}, std::move(x)}[2_c] == 42.8f);
    }
    {
        fcc::tuple<std::string> x{"hi"};
        CHECK(fcc::tuple<std::string>{x}[0_c] == "hi");
        CHECK(!noexcept(fcc::tuple<std::string>{x}));
        CHECK(fcc::tuple<std::string>{std::move(x)}[0_c] == "hi");
        CHECK(noexcept(fcc::tuple<std::string>{std::move(x)}));
        
    }
    return test_result();
}
