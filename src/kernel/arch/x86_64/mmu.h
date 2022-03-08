#pragma once

#include <kernel/vmm.h>
#include <utils/base/attributes.h>
#include <utils/base/defs.h>
#include <utils/base/range.h>

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
} pml_t;
static_assert(sizeof(pml_t) == 8, "pml_t was not 64-bits");

static inline pml_t pml_new(pm_addr_t addr, uint32_t flags) {
	return (pml_t){
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

static inline pml_t pml_empty(void) {
	return (pml_t){0};
}

typedef struct {
	pml_t entries[512];
} ALIGNED(MEM_PAGE_SIZE) pml_table_t;
