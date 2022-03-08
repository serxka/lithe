#pragma once

#include <utils/base/stdinc.h>

#define COMPILER_CLANG 0
#define COMPILER_GCC 0

#if defined(__GNUC__) && defined(__clang__)
#undef COMPILER_CLANG
#define COMPILER_CLANG 1
#elif defined(__GNUC__)
#undef COMPILER_GCC
#define COMPILER_GCC 1
#else
#error "unsupported compiler"
#endif

#define null ((void*)0)
#define static_assert _Static_assert

#define _VARGS(_10, _9, _8, _7, _6, _5, _4, _3, _2, _1, N, ...) N
#define VARGS(...) _VARGS(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define _CONCAT(a, b) a##b
#define CONCAT(a, b) _CONCAT(a, b)

#define MACRO_OVERLOAD(BASE, ...) CONCAT(BASE, VARGS(__VA_ARGS__))
// Use MACRO_OVERLOAD like below
// 	#define SUM_2(a, b) (a) + (b)
// 	#define SUM_1(a) SUM_2((a), 0)
// 	#define SUM(...) MACRO_OVERLOAD(SUM_, __VA_ARGS__)(__VA_ARGS__)

typedef char symbol_t[];
