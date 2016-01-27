//  Copyright xcvr (mtak-) 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
//
// Project home: https://github.com/mtak-/tuple
//

#ifndef FCC_TUPLE_TUPLE_IO_HPP
#define FCC_TUPLE_TUPLE_IO_HPP

#include <fcc/tuple/tuple_algorithm.hpp>

namespace fcc {
    ////////////////////////////
    // pretty(TupleLike&& tup)
    ////////////////////////////
    struct pretty_fn {
    private:
        // workaround literal in return type
        template<typename Tup>
        struct return_type
            : meta::id<decltype(tuple_cat(make_tuple((const char*)"("),
                                          intersperse((const char*)", ", std::declval<Tup>()),
                                          make_tuple((const char*)")")))> {};
    public:
        template<TupleLike Tup>
        constexpr meta::_t<return_type<Tup>> operator()(Tup&& tup) const
        FCC_RETURN_NOEXCEPT(
            tuple_cat(make_tuple((const char*)"("),
                      intersperse((const char*)", ", std::forward<Tup>(tup)),
                      make_tuple((const char*)")"))
        )
    };
    namespace { constexpr auto&& pretty = static_const<pretty_fn>::value; }
    
    ////////////
    // CString
    ////////////
    template<typename T>
    concept bool CString = std::is_same<std::decay_t<T>, const char*>::value;
    
    //////////////////////////////////////
    // operator>>(istream&, TupleLike&&)
    //////////////////////////////////////
    namespace detail {
        template<typename Istr, typename Tup, typename ISeq, typename = void>
        struct is_tuple_istreamable_ : std::false_type { using result = void; };
        
        template<Istream Istr, TupleLike Tup, std::size_t... Is> requires !CString<Tup>
        struct is_tuple_istreamable_<Istr, Tup, meta::iseq<Is...>,
            meta::void_<decltype((std::declval<Istr>() >> ... >> get<Is>(std::declval<Tup>())))>>
            : std::true_type
        { using result = decltype((std::declval<Istr>() >> ... >> get<Is>(std::declval<Tup>()))); };
    }
    
    template<typename Istr, typename Tup>
    struct is_tuple_istreamable : std::false_type { using result = void; };
    
    template<Istream Istr, TupleLike Tup>
    struct is_tuple_istreamable<Istr, Tup>
        : detail::is_tuple_istreamable_<Istr, Tup, tuple_indices<Tup>> {};
    
    template<typename Istr, typename Tup>
    concept bool TupleIstreamable = is_tuple_istreamable<Istr, Tup>::value;
    
    namespace detail {
        template<typename Istr, typename Tup>
        using istream_result = typename is_tuple_istreamable<Istr, Tup>::result;

        template<typename Istr, typename Tup, std::size_t... Is>
        constexpr istream_result<Istr, Tup> stream_in(Istr&& istr, Tup&& tup, meta::iseq<Is...>)
        FCC_RETURN_NOEXCEPT(
            (std::forward<Istr>(istr) >> ... >> get<Is>(std::forward<Tup>(tup)))
        )
    }

    inline namespace operators {
        template<typename Istr, typename Tup> requires fcc::TupleIstreamable<Istr, Tup>
        constexpr fcc::detail::istream_result<Istr, Tup> operator>>(Istr&& istr, Tup&& tup)
        FCC_RETURN_NOEXCEPT(
            fcc::detail::stream_in(std::forward<Istr>(istr), std::forward<Tup>(tup),
                                   fcc::tuple_indices<Tup>{})
        )
    }
    
    //////////////////////////////////////
    // operator<<(ostream&, TupleLike&&)
    //////////////////////////////////////
    namespace detail {
        template<typename Ostr, typename Tup, typename ISeq, typename = void>
        struct is_tuple_ostreamable_ : std::false_type { using result = void; };
        
        template<Ostream Ostr, TupleLike Tup, std::size_t... Is> requires !CString<Tup>
        struct is_tuple_ostreamable_<Ostr, Tup, meta::iseq<Is...>,
            meta::void_<decltype((std::declval<Ostr>() << ... << get<Is>(std::declval<Tup>())))>>
            : std::true_type
        { using result = decltype((std::declval<Ostr>() << ... << get<Is>(std::declval<Tup>()))); };
    }
    
    template<typename Ostr, typename Tup>
    struct is_tuple_ostreamable : std::false_type { using result = void; };
    
    template<Ostream Ostr, TupleLike Tup>
    struct is_tuple_ostreamable<Ostr, Tup>
        : detail::is_tuple_ostreamable_<Ostr, Tup, tuple_indices<Tup>> {};
    
    template<typename Ostr, typename Tup>
    concept bool TupleOstreamable = is_tuple_ostreamable<Ostr, Tup>::value;
    
    namespace detail {
        template<typename Ostr, typename Tup>
        using ostream_result = typename is_tuple_ostreamable<Ostr, Tup>::result;

        template<typename Ostr, typename Tup, std::size_t... Is>
        constexpr ostream_result<Ostr, Tup> stream_out(Ostr&& ostr, Tup&& tup, meta::iseq<Is...>)
        FCC_RETURN_NOEXCEPT(
            (std::forward<Ostr>(ostr) << ... << get<Is>(std::forward<Tup>(tup)))
        )
    }

    inline namespace operators {
        template<typename Ostr, typename Tup> requires fcc::TupleOstreamable<Ostr, Tup>
        constexpr fcc::detail::ostream_result<Ostr, Tup> operator<<(Ostr&& ostr, Tup&& tup)
        FCC_RETURN_NOEXCEPT(
            fcc::detail::stream_out(std::forward<Ostr>(ostr), std::forward<Tup>(tup),
                                    fcc::tuple_indices<Tup>{})
        )
    }
}

#endif
