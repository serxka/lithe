#pragma once

#define ALIGN_UP(X, ALIGN) ((X) + ((ALIGN)-1) & ~((ALIGN)-1))
#define ALIGN_DOWN(X, ALIGN) ((X) ~((ALIGN)-1))

#define MIN(X, Y)                             \
	({                                    \
		typeof(X) _x = (X), _y = (Y); \
		_x < _y ? _x : _y;            \
	})
#define MAX(X, Y)                             \
	({                                    \
		typeof(X) _x = (X), _y = (Y); \
		_x > _y ? _x : _y;            \
	})

#define SWAP(X, Y)                  \
	do {                        \
		typeof(X) _z = (X); \
		(X) = (Y);          \
		(Y) = _z;           \
	} while (0)
