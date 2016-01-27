//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_TEST_TEST_TYPES_HPP
#define FCC_TEST_TEST_TYPES_HPP

struct empty {};

struct no_default_constructor
{
    no_default_constructor(empty) {}
};

struct no_copy
{
    no_copy() {}
    
    no_copy(const no_copy&) = delete;
    no_copy& operator=(const no_copy&) = delete;
    
    no_copy(no_copy&&) {}
    no_copy& operator=(no_copy&&) { return *this; }
};

struct no_move
{
    no_move() {}
    
    no_move(const no_move&) {}
    no_move& operator=(const no_move&) { return *this; }
    
    no_move(no_move&&) = delete;
    no_move& operator=(no_move&&) = delete;
};

struct no_copy_or_move
{
    no_copy_or_move() {}
    
    no_copy_or_move(const no_copy_or_move&) = delete;
    no_copy_or_move& operator=(const no_copy_or_move&) = delete;
    
    no_copy_or_move(no_copy_or_move&&) = delete;
    no_copy_or_move& operator=(no_copy_or_move&&) = delete;
};

struct nothing
{
    nothing() = delete;
    
    nothing(const nothing&) = delete;
    nothing& operator=(const nothing&) = delete;
    
    nothing(nothing&&) = delete;
    nothing& operator=(nothing&&) = delete;
};

struct volatile_rref_constructor
{
    int i = 42;
    volatile_rref_constructor() noexcept {}
    
    volatile_rref_constructor(const volatile_rref_constructor&) = delete;
    volatile_rref_constructor& operator=(const volatile_rref_constructor&) = delete;
    
    volatile_rref_constructor(volatile_rref_constructor&&) = delete;
    volatile_rref_constructor& operator=(volatile_rref_constructor&&) = delete;
    
    volatile_rref_constructor(volatile volatile_rref_constructor&&) noexcept { i = 43; }
};

// very weird type
struct rvalue_only_assign
{
    explicit rvalue_only_assign() {}
    
    explicit rvalue_only_assign(const rvalue_only_assign&) {}
    rvalue_only_assign& operator=(const rvalue_only_assign&) requires false = delete;
    
    explicit rvalue_only_assign(rvalue_only_assign&&) {}
    rvalue_only_assign& operator=(rvalue_only_assign&&) requires false = delete;
    
    rvalue_only_assign&& operator=(rvalue_only_assign&&) && { return std::move(*this); }
};

#endif
