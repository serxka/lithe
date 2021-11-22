#pragma once

#include <lithe/base/litheinc.h>

#define bswap(VAL)                                       \
	({                                               \
		typeof(VAL) _val = VAL;                  \
		typeof(VAL) _res = 0;                    \
		if (sizeof(_val) == 1)                   \
			_res = _val;                     \
		if (sizeof(_val) == 2)                   \
			_res = __builtin__bswap16(_val); \
		if (sizeof(_val) == 4)                   \
			_res = __builtin__bswap32(_val); \
		if (sizeof(_val) == 8)                   \
			_res = __builtin__bswap64(_val); \
		_val;                                    \
	})

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define endian_swap_little(VAL) (VAL)
#else
#define endian_swap_little(VAL) bswap(VAL)
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define endian_swap_big(VAL) bswap(VAL)
#else
#define endian_swap_big(VAL) (VAL)
#endif
