#pragma once

#include <utils/base/defs.h>

#define range_t(T)      \
	struct {        \
		T base; \
		T size; \
	}

#define range$(T, BASE, SIZE) ((T){BASE, SIZE})

#define range_empty(R) ((R).size == 0)
#define range_start(R) ((R).base)
#define range_end(R) ((R).base + (R).size)
#define range_equ(R1, R2) ((R1).base == (R2).base && (R1).size == (R2).size)

#define range_is_page_aligned(R) ((R).base % 4096 == 0 && (R).size % 4096 == 0)

typedef range_t(uint8_t) range_u8_t;
typedef range_t(uint8_t) range_u16_t;
typedef range_t(uint32_t) range_u32_t;
typedef range_t(uint64_t) range_u64_t;
typedef range_t(uintptr_t) range_usize_t;
