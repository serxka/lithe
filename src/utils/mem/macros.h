#pragma once

#define align_up$(X, ALIGN) ((X) + ((ALIGN)-1) & ~((ALIGN)-1))
#define align_down$(X, ALIGN) ((X) & ~((ALIGN)-1))

#define length$(X) (sizeof(X) / sizeof((X)[0]))

#define min$(X, Y)                            \
	({                                    \
		typeof(X) _x = (X), _y = (Y); \
		_x < _y ? _x : _y;            \
	})
#define max$(X, Y)                            \
	({                                    \
		typeof(X) _x = (X), _y = (Y); \
		_x > _y ? _x : _y;            \
	})

#define swap$(X, Y)                 \
	do {                        \
		typeof(X) _z = (X); \
		(X) = (Y);          \
		(Y) = _z;           \
	} while (0)

// Units to use for memory
#define KiB(K) (1024ULL * (K))
#define MiB(M) (KiB(1024ULL * (M)))
#define GiB(G) (MiB(1024ULL * (G)))
