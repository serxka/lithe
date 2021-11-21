#ifndef _KERNEL_TYPES_H_
#define _KERNEL_TYPES_H_

#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ARR_LEN(x) (sizeof(x) / sizeof((x)[0]))
#define UNUSED(x) (void)(x)

#define PACKED __attribute__((packed))
#define ALIGNED(x) __attribute__((aligned(x)))
#define noreturn _Noreturn
#define static_assert(e, m) _Static_assert(e, m)

typedef char symbol_t[];

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#endif // _KERNEL_TYPES_H_
