#pragma once

#include <lithe/base/stdinc.h>

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

typedef char symbol_t[];
