#pragma once

#define ALIGNED(align) __attribute__((aligned(align)))
#define DEFER(func) __attribute__((cleanup(func)))
#define INLINE inline __attribute__((always_inline))
#define PACKED __attribute__((packed))
#define UNUSED(expr) ((void)(expr))
