#pragma once

#include <utils/base/stdinc.h>

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
#define little_endian_swap(VAL) (VAL)
#else
#define little_endian_swap(VAL) bswap(VAL)
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define big_endian_swap(VAL) bswap(VAL)
#else
#define big_endian_swap(VAL) (VAL)
#endif
