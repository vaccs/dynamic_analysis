// Copyright (c) 2013 Austin T. Clements. All rights reserved.
// Use of this source code is governed by an MIT license
// that can be found in the LICENSE file.

#ifndef _ELFPP_COMMON_HH_
#define _ELFPP_COMMON_HH_

#define ELFPP_BEGIN_NAMESPACE namespace elf {
#define ELFPP_END_NAMESPACE   }
#define ELFPP_BEGIN_INTERNAL  namespace internal {
#define ELFPP_END_INTERNAL    }

#include <inttypes.h>

ELFPP_BEGIN_NAMESPACE

/**
 * A byte ordering.
 */
typedef int byte_order;
namespace byte_order_ns {
        typedef int byte_order;
        const byte_order native = 0;
        const byte_order lsb = 1;
        const byte_order msb = 2;
};

/**
 * Return either byte_order_ns::lsb or byte_order_ns::msb.  If the argument
 * is byte_order_ns::native, it will be resolved to whatever the native
 * byte order is.
 */
static inline byte_order
resolve_order(byte_order o)
{
        static const union
        {
                int i;
                char c[sizeof(int)];
        } test = {1};

        if (o == byte_order_ns::native)
                return test.c[0] == 1 ? byte_order_ns::lsb : byte_order_ns::msb;
        return o;
}

/**
 * Return v converted from one byte order to another.
 */
template<typename T>
T
swizzle(T v, byte_order from, byte_order to)
{
        // c++03 style static assert
        typedef int static_assert_can_swizzle[sizeof(T) == 1 ||
                                              sizeof(T) == 2 ||
                                              sizeof(T) == 4 ||
                                              sizeof(T) == 8 ? 1 : -1];

        from = resolve_order(from);
        to = resolve_order(to);

        if (from == to)
                return v;

        switch (sizeof(T)) {
        case 1:
                return v;
        case 2: {
                uint16_t x = (uint16_t)v;
                return (T)(((x&0xFF) << 8) | (x >> 8));
        }
        case 4:
                return (T)__builtin_bswap32((uint32_t)v);
        case 8:
                return (T)__builtin_bswap64((uint64_t)v);
        }
}

ELFPP_BEGIN_INTERNAL

/**
 * OrderPick selects between Native, LSB, and MSB based on ord.
 */
template<byte_order ord, typename Native, typename LSB, typename MSB>
struct OrderPick;

template<typename Native, typename LSB, typename MSB>
struct OrderPick<byte_order_ns::native, Native, LSB, MSB>
{
        typedef Native T;
};

template<typename Native, typename LSB, typename MSB>
struct OrderPick<byte_order_ns::lsb, Native, LSB, MSB>
{
        typedef LSB T;
};

template<typename Native, typename LSB, typename MSB>
struct OrderPick<byte_order_ns::msb, Native, LSB, MSB>
{
        typedef MSB T;
};

ELFPP_END_INTERNAL

ELFPP_END_NAMESPACE

#endif
