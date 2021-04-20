// Copyright (c) 2013 Austin T. Clements. All rights reserved.
// Use of this source code is governed by an MIT license
// that can be found in the LICENSE file.

#ifndef _ELFPP_TO_HEX_HH_
#define _ELFPP_TO_HEX_HH_

#include <string>
#include <type_traits>

#include "../dwarf/cpp98help.hh"

template<typename T>
std::string
to_hex(T v)
{
        // c++98 style static assert
        typedef int static_assert_integral[is_integral<T>::value ? 1 : -1];
        if (v == 0)
                return std::string("0");
        char buf[sizeof(T)*2 + 1];
        char *pos = &buf[sizeof(buf)-1];
        *pos-- = '\0';
        while (v && pos >= buf) {
                int digit = v & 0xf;
                if (digit < 10)
                        *pos = '0' + digit;
                else
                        *pos = 'a' + (digit - 10);
                pos--;
                v >>= 4;
        }
        return std::string(pos + 1);
}

#endif // _ELFPP_TO_HEX_HH_
