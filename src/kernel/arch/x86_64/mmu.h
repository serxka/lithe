#pragma once

#include <kernel/vmm.h>
#include <lithe/base/attributes.h>
#include <lithe/base/defs.h>
#include <lithe/base/range.h>

#define MEM_PAGE_SHIFT (12)
#define MEM_PAGE_SHIFT_MED (21)
#define MEM_PAGE_SHIFT_BIG (30)

#define MEM_PAGE_SIZE (0x1 << MEM_PAGE_SHIFT)
#define MEM_PAGE_SIZE_MED (0x1 << MEM_PAGE_SHIFT_MED)
#define MEM_PAGE_SIZE_BIG (0x1 << MEM_PAGE_SHIFT_BIG)

#define MEM_PAGE_MASK (0x1FFF)

typedef union {
	struct {
		uint64_t present : 1;
		uint64_t writable : 1;
		uint64_t user : 1;
		uint64_t writethrough : 1;
		uint64_t nocache : 1;
		uint64_t accessed : 1;
		uint64_t dirty : 1;
		uint64_t size : 1;
		uint64_t global : 1;
		uint64_t _i : 3;
		uint64_t pat : 1;
		uint64_t phys : 50;
		uint64_t xd : 1;
	};
	uint64_t _raw;
} pml;
static_assert(sizeof(pml) == 8, "pml was not 64-bits");

static inline pml pml_new(pm_addr addr, uint32_t flags) {
	return (pml){
		.present = 1,
		.writable = flags & MEM_WRITABLE ? 1 : 0,
		.user = flags * MEM_USER ? 1 : 0,
		.phys = addr >> MEM_PAGE_SHIFT,
		.writethrough = 0,
		.nocache = 0,
		.accessed = 0,
		.dirty = 0,
		.size = 0,
		.global = 0,
		._i = 0,
		.pat = 0,
		.xd = 0,
	};
}

typedef struct {
	pml entries[512];
} ALIGNED(MEM_PAGE_SIZE) pml_table;
