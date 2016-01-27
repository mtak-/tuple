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

template<typename... Us, typename... Ts>
void test_copies(Ts&&... ts)
{
    auto lhs = fcc::tuple<Us...>{{std::forward<Ts>(ts)}...};
    CHECK(lhs == fcc::tuple<Us...>{std::forward<Ts>(ts)...});
    CHECK(noexcept(fcc::tuple<Us...>{std::forward<Ts>(ts)...}) ==
          meta::and_c<std::is_nothrow_constructible<Us, Ts&&>::value...>::value);
}

int main() {
    {
        test_copies<int, double, float, std::string>(5, 10.3, 5.8f, "ohi");
        test_copies<>();
    }
    
    {
        int x;
        test_copies<int&>(x);
        test_copies<const int&>(x);
        test_copies<volatile int&>(x);
        test_copies<const volatile int&>(x);
    }
    
    {
        test_copies<int&&>(5);
        test_copies<const int&&>(5);
        test_copies<volatile int&&>(5);
        test_copies<const volatile int&&>(5);
    }
    
    {
        std::string x{"HI"};
        NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<std::string&>{{x}}).size() == 2u);
        NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<const std::string&>{{x}}).size() == 2u);
        CHECK(noexcept(fcc::tuple<volatile std::string&>{{x}}));
        CHECK(noexcept(fcc::tuple<const volatile std::string&>{{x}}));
    }
    
    CHECK(fcc::get<1>(fcc::tuple<std::vector<int>, int&&>{{0, 0, 0, 0}, 9}) == 9);
    CHECK(fcc::get<0>(fcc::tuple<std::unique_ptr<int>>{{nullptr}}) == nullptr);
    CHECK(noexcept(fcc::get<0>(fcc::tuple<std::unique_ptr<int>>{{nullptr}})));
    
    {
        fcc::tuple<empty> y{{}};
        fcc::tuple<empty, empty> z{{}, {}};
        fcc::tuple<empty, empty, empty> w{{}, {}, {}};
    }
    
    {
        NOEXCEPT_CHECK(fcc::get<0>(fcc::tuple<int>{fcc::allocator_arg, std::allocator<int>{}, {5}}) == 5);
        EXCEPT_CHECK(fcc::get<0>(fcc::tuple<std::vector<int>>{fcc::allocator_arg, std::allocator<int>{}, {5}}).size() == 1u);
    }
    
    {
        fcc::tuple<std::vector<int>> x{fcc::allocator_arg, std::allocator<int>{}, {42}};
        CHECK(fcc::get<0>(x)[0] == 42);
        CHECK(fcc::get<0>(x).size() == 1u);
    }
    
    {
        fcc::tuple<>{fcc::allocator_arg, std::allocator<int>{}};
    }

    return test_result();
}