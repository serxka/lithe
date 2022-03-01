#pragma once

#include <kernel/vm.h>
#include <lithe/base/attributes.h>
#include <lithe/base/defs.h>
#include <lithe/base/range.h>

#define MEM_PAGE_SHIFT (12)
#define MEM_PAGE_SHIFT_MED (21)
#define MEM_PAGE_SHIFT_BIG (30)

#define MEM_PAGE_SIZE (0x1 << MEM_PAGE_SHIFT)
#define MEM_PAGE_SIZE_MED (0x1 << MEM_PAGE_SHIFT_MED)
#define MEM_PAGE_SIZE_BIG (0x1 << MEM_PAGE_SHIFT_BIG)

#define MEM_PAGE_MASK (0x1FF)

typedef union {
	struct {
		bool present : 1;
		bool writable : 1;
		bool user : 1;
		bool writethrough : 1;
		bool nocache : 1;
		bool accessed : 1;
		bool dirty : 1;
		bool size : 1;
		bool global : 1;
		uint64_t _i : 3;
		uint64_t phys : 51;
		bool xd : 1;
	};
	uint64_t _raw;
} pml;
static_assert(sizeof(pml) == 8, "pml was not 64-bits");

static inline pml pml_new(pm_addr addr, uint32_t flags) {
	return (pml){
	        .present = true,
	        .writable = flags & MEM_WRITABLE ? true : false,
	        .user = flags & MEM_USER ? true : false,
	        .writethrough = false,
	        .nocache = false,
	        .accessed = false,
	        .dirty = false,
	        .size = false,
	        .global = false,
	        .phys = addr >> MEM_PAGE_SHIFT,
	        .xd = false,
	        ._i = 0,
	};
}

static inline pml pml_empty(void) {
	return (pml){0};
}

typedef struct {
	pml entries[512];
} ALIGNED(MEM_PAGE_SIZE) pml_table;
