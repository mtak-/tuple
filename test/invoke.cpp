// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//

#include <fcc/invoke.hpp>

#include <memory>

#include "simple_test.hpp"

constexpr struct {
  constexpr auto&& operator()(auto&& arg) const noexcept {
    return (decltype(arg)&&)arg;
  }
} h;

struct A {
  int i = 13;
  constexpr int f() const noexcept { return 42; }
  constexpr int g(int i) { return 2 * i; }
};

constexpr int f() noexcept { return 13; }
constexpr int g(int i) { return 2 * i + 1; }

int main() {
  CHECK(fcc::invoke(f) == 13);
  CHECK(noexcept(fcc::invoke(f) == 13));
  CHECK(fcc::invoke(g, 2) == 5);
  CHECK(fcc::invoke(h, 42) == 42);
  CHECK(noexcept(fcc::invoke(h, 42) == 42));
  {
    int i = 13;
    CHECK(&fcc::invoke(h, i) == &i);
    CHECK(noexcept(&fcc::invoke(h, i) == &i));
  }

  CHECK(fcc::invoke(&A::f, A{}) == 42);
  CHECK(noexcept(fcc::invoke(&A::f, A{}) == 42));
  CHECK(fcc::invoke(&A::g, A{}, 2) == 4);
  {
    A a;
    const auto& ca = a;
    CHECK(fcc::invoke(&A::f, a) == 42);
    CHECK(noexcept(fcc::invoke(&A::f, a) == 42));
    CHECK(fcc::invoke(&A::f, ca) == 42);
    CHECK(noexcept(fcc::invoke(&A::f, ca) == 42));
    CHECK(fcc::invoke(&A::g, a, 2) == 4);
  }

  {
    A a;
    const auto& ca = a;
    CHECK(fcc::invoke(&A::f, &a) == 42);
    CHECK(noexcept(fcc::invoke(&A::f, &a) == 42));
    CHECK(fcc::invoke(&A::f, &ca) == 42);
    CHECK(noexcept(fcc::invoke(&A::f, &ca) == 42));
    CHECK(fcc::invoke(&A::g, &a, 2) == 4);
  }
  {
    auto up = std::make_unique<A>();
    CHECK(fcc::invoke(&A::f, up) == 42);
    CHECK(fcc::invoke(&A::g, up, 2) == 4);
  }
  {
    auto sp = std::make_shared<A>();
    CHECK(fcc::invoke(&A::f, sp) == 42);
    CHECK(noexcept(fcc::invoke(&A::f, sp) == 42));
    CHECK(fcc::invoke(&A::g, sp, 2) == 4);
  }

  CHECK(fcc::invoke(&A::i, A{}) == 13);
  CHECK(noexcept(fcc::invoke(&A::i, A{}) == 13));
  { int&& tmp = fcc::invoke(&A::i, A{}); (void)tmp; }

  {
    A a;
    const auto& ca = a;
    CHECK(fcc::invoke(&A::i, a) == 13);
    CHECK(noexcept(fcc::invoke(&A::i, a) == 13));
    CHECK(fcc::invoke(&A::i, ca) == 13);
    CHECK(noexcept(fcc::invoke(&A::i, ca) == 13));
    CHECK(fcc::invoke(&A::i, &a) == 13);
    CHECK(noexcept(fcc::invoke(&A::i, &a) == 13));
    CHECK(fcc::invoke(&A::i, &ca) == 13);
    CHECK(noexcept(fcc::invoke(&A::i, &ca) == 13));

    fcc::invoke(&A::i, a) = 0;
    CHECK(a.i == 0);
    fcc::invoke(&A::i, &a) = 1;
    CHECK(a.i == 1);
    static_assert(std::is_same<decltype(fcc::invoke(&A::i, ca)), const int&>{});
    static_assert(std::is_same<decltype(fcc::invoke(&A::i, &ca)), const int&>{});
  }

  {
    auto up = std::make_unique<A>();
    CHECK(fcc::invoke(&A::i, up) == 13);
    fcc::invoke(&A::i, up) = 0;
    CHECK(up->i == 0);
  }

  {
    auto sp = std::make_shared<A>();
    CHECK(fcc::invoke(&A::i, sp) == 13);
    fcc::invoke(&A::i, sp) = 0;
    CHECK(sp->i == 0);
  }
  
  {
    struct B { int i = 42; constexpr int f() const { return i; } };
    constexpr B b;
    static_assert(b.i == 42);
    static_assert(b.f() == 42);
    static_assert(fcc::invoke(&B::i, b) == 42);
    static_assert(fcc::invoke(&B::i, &b) == 42);
    static_assert(fcc::invoke(&B::i, B{}) == 42);
    static_assert(fcc::invoke(&B::f, b) == 42);
    static_assert(fcc::invoke(&B::f, &b) == 42);
    static_assert(fcc::invoke(&B::f, B{}) == 42);
  }

  return ::test_result();
}