#pragma once

#include <lithe/base/defs.h>

#define range(T)        \
	struct {        \
		T base; \
		T size; \
	}

#define range$(T, BASE, SIZE) ((T){BASE, SIZE})

#define range_empty(R) ((R).size == 0)
#define range_start(R) ((R).base)
#define range_end(R) ((R).base + (R).end)
#define range_equ(R1, R2) ((R1).base == (R2).base && (R1).size == (R2).size)

typedef range(uint8_t) range_u8;
typedef range(uint8_t) range_u16;
typedef range(uint32_t) range_u32;
typedef range(uint64_t) range_u64;
typedef range(size_t) range_usize;
