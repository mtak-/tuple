//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_INVOKE_HPP
#define FCC_INVOKE_HPP

#include <fcc/detail/returns.hpp>
#include <fcc/detail/static_const.hpp>

#include <meta/meta.hpp>

#include <functional>
#include <type_traits>

namespace fcc
{
    // the following is entirely based on the sample implementation in cppreference
    // http://en.cppreference.com/w/cpp/utility/functional/invoke
    // constexpr, noexcept, requires, and style changes added
    namespace detail {
        template<typename T>
        struct is_reference_wrapper : std::false_type {};
        template<typename U>
        struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type {};
         
        template<typename Base, typename T, typename Derived, typename... Args>
            requires std::is_function<T>::value &&
                     std::is_base_of<Base, std::decay_t<Derived>>::value
        constexpr auto invoke_impl(T Base::*pmf, Derived&& ref, Args&&... args)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            (std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...)
        )
         
        template<typename Base, typename T, typename RefWrap, typename... Args>
            requires std::is_function<T>::value &&
                     is_reference_wrapper<meta::uncvref<RefWrap>>::value
        constexpr auto invoke_impl(T Base::*pmf, RefWrap&& ref, Args&&... args)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            (ref.get().*pmf)(std::forward<Args>(args)...)
        )
         
        template<typename Base, typename T, typename Pointer, typename... Args>
            requires std::is_function<T>::value &&
                     !is_reference_wrapper<meta::uncvref<Pointer>>::value &&
                     !std::is_base_of<Base, std::decay_t<Pointer>>::value
        constexpr auto invoke_impl(T Base::*pmf, Pointer&& ptr, Args&&... args)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            ((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...)
        )
        
        template<typename Base, typename T, typename Derived>
            requires !std::is_function<T>::value &&
                     std::is_base_of<Base, std::decay_t<Derived>>::value
        constexpr auto invoke_impl(T Base::*pmd, Derived&& ref)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            std::forward<Derived>(ref).*pmd
        )
        
        template<typename Base, typename T, typename RefWrap>
            requires !std::is_function<T>::value &&
                     is_reference_wrapper<meta::uncvref<RefWrap>>::value
        constexpr auto invoke_impl(T Base::*pmd, RefWrap&& ref)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            std::forward<RefWrap>(ref).get().*pmd
        )
         
        template<typename Base, typename T, typename Pointer>
            requires !std::is_function<T>::value &&
                     !is_reference_wrapper<meta::uncvref<Pointer>>::value &&
                     !std::is_base_of<Base, std::decay_t<Pointer>>::value
        constexpr auto invoke_impl(T Base::*pmd, Pointer&& ptr)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            (*std::forward<Pointer>(ptr)).*pmd
        )
        
        template<typename F, typename... Args>
            requires !std::is_member_pointer<std::decay_t<F>>::value
        constexpr auto invoke_impl(F&& f, Args&&... args)
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            std::forward<F>(f)(std::forward<Args>(args)...)
        )
    } // namespace detail
    
    struct invoke_fn {
        template<typename F, typename... ArgTypes>
        constexpr auto operator()(F&& f, ArgTypes&&... args) const
        FCC_DECLTYPE_AUTO_RETURN_NOEXCEPT(
            detail::invoke_impl(std::forward<F>(f), std::forward<ArgTypes>(args)...)
        )
    };
    namespace { constexpr auto&& invoke = static_const<invoke_fn>::value; }
    
    namespace detail {
        template<typename F, typename List, typename = void>
        struct is_invokable_ : std::false_type {};
        
        template<typename F, typename... ArgTypes>
        struct is_invokable_<F, meta::list<ArgTypes...>,
                             meta::void_<decltype(::fcc::invoke(std::declval<F>(),
                                                                std::declval<ArgTypes>()...))>>
            : std::true_type
        {};
    }
    
    template<typename F, typename... ArgTypes>
    struct is_invokable : detail::is_invokable_<F, meta::list<ArgTypes...>>::type {};
    
    template<typename F, typename... ArgTypes>
    concept bool Invokable = ::fcc::is_invokable<F, ArgTypes...>::value;
    
    template<typename F, typename... ArgTypes>
    struct is_nothrow_invokable : std::false_type {};
    
    template<typename F, typename... ArgTypes>
        requires Invokable<F, ArgTypes...>
    struct is_nothrow_invokable<F, ArgTypes...>
        : meta::bool_<noexcept(::fcc::invoke(std::declval<F>(), std::declval<ArgTypes>()...))> {};
    
    template<typename F, typename... ArgTypes>
    concept bool NothrowInvokable = ::fcc::is_nothrow_invokable<F, ArgTypes...>::value;
}

#endif
