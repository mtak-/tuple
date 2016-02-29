/*!
@file
Defines `boost::hana::fold_left`.
@copyright Louis Dionne 2013-2016
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef FCC_DETAIL_LITERALS_HPP
#define FCC_DETAIL_LITERALS_HPP

#include <meta/meta.hpp>

// 99.9999999999% copy paste from boost hana
namespace fcc {
    namespace detail {
        constexpr int to_int(char c) {
            int result = 0;

            if (c >= 'A' && c <= 'F') {
                result = static_cast<int>(c) - static_cast<int>('A') + 10;
            }
            else if (c >= 'a' && c <= 'f') {
                result = static_cast<int>(c) - static_cast<int>('a') + 10;
            }
            else {
                result = static_cast<int>(c) - static_cast<int>('0');
            }

            return result;
        }

        template<std::size_t N>
        constexpr std::size_t parse(const char (&arr)[N]) {
            std::size_t base = 10;
            std::size_t offset = 0;

            if (N > 2) {
                bool starts_with_zero = arr[0] == '0';
                bool is_hex = starts_with_zero && arr[1] == 'x';
                bool is_binary = starts_with_zero && arr[1] == 'b';

                if (is_hex) {
                    //0xDEADBEEF (hexadecimal)
                    base = 16;
                    offset = 2;
                }
                else if (is_binary) {
                    //0b101011101 (binary)
                    base = 2;
                    offset = 2;
                }
                else if (starts_with_zero) {
                    //012345 (octal)
                    base = 8;
                    offset = 1;
                }
            }

            std::size_t number = 0;
            std::size_t multiplier = 1;

            for (std::size_t i = 0; i < N - offset; ++i) {
                char c = arr[N - 1 - i];
                number += to_int(c) * multiplier;
                multiplier *= base;
            }

            return number;
        }
    }

    inline namespace literals {
        template <char ...c>
        constexpr auto operator"" _c() noexcept
        { return meta::size_t<detail::parse<sizeof...(c)>({c...})>{}; }
    }
}

#endif /* literals_h */
