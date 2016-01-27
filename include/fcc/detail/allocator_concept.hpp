//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_DETAIL_ALLOCATOR_CONCEPT_HPP
#define FCC_DETAIL_ALLOCATOR_CONCEPT_HPP

#include <meta/meta.hpp>

#include <memory>

// the allocator concept is pretty ridiculous
// this is as good as i feel like for now
namespace fcc {
    namespace detail {
        struct nat {};
        
        template<typename Alloc>
        concept bool AllocatorDetails = requires(Alloc a) {
            typename Alloc::value_type;
            {*(typename std::allocator_traits<Alloc>::pointer{})} -> typename Alloc::value_type&;
            {*(typename std::allocator_traits<Alloc>::const_pointer{})} -> const typename Alloc::value_type&;
            {a.allocate(1)} -> typename std::allocator_traits<Alloc>::pointer;
            a.deallocate(std::declval<typename std::allocator_traits<Alloc>::pointer>(), 1);
            a == a;
            a != a;
            Alloc(a);
            Alloc(std::move(a));
            Alloc(typename std::allocator_traits<Alloc>::template rebind_alloc<detail::nat>{});
        };
        
        template<typename T>
        struct AllocatorImpl : std::false_type {};
        
        template<AllocatorDetails T>
        struct AllocatorImpl<T> : std::true_type {};
        
        template<typename T, bool = std::is_class<T>::value || std::is_union<T>::value>
        struct Allocator_ : std::false_type {};
        
        template<typename T>
        struct Allocator_<T, true> : AllocatorImpl<T> {};
    }

    template<typename Alloc>
    concept bool UnqualifiedAllocator = detail::Allocator_<Alloc>::value;
    
    template<typename Alloc>
    concept bool Allocator = UnqualifiedAllocator<meta::uncvref<Alloc>>;
    
    template<typename Alloc>
    struct is_allocator : std::false_type {};
    
    template<typename Alloc> requires UnqualifiedAllocator<std::remove_cv_t<Alloc>>
    struct is_allocator<Alloc> : std::true_type {};
}

#endif
