#include <string.h>

#include <kernel/kprintf.h>
#include <kernel/spinlock.h>
#include <kernel/types.h>

#include "include/mmu.h"

#define PAGE_SHIFT (12)
#define PAGE_SIZE (0x1 < PAGE_SHIFT)

#deinfe LARGE_PAGE_SIZE (0x1 << 21) /*2MiB*/

#define PAGE_KERNEL_FLAG 0x03
#define PAGE_SIZE_FLAG 0x80

#define INDEX_FROM_BIT(x) ((x) >> 6)
#define OFFSET_FROM_BIT(x) ((x) & 0x3F)

uint64_t *frames;
uint32_t nframes;

static void mmu_frame_set(uintptr_t addr) {
	uint32_t frame = addr >> PAGE_SHIFT;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

static void mmu_frame_clear(uintptr_t addr) {
	uint32_t frame = addr >> PAGE_SHIFT;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}

static bool mmu_frame_test(uintptr_t addr) {
	uint32_t frame = addr >> PAGE_SHIFT;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	return frames[idx] & (0x1 << off);
}

static uint32_t mmu_first_frames(uint32_t n) {
	for (uintptr_t a = 0; a < nframes * PAGE_SIZE; a += PAGE_SIZE) {
		uintptr_t hole = 0;
		for (uintptr_t j = 0; j < n * PAGE_SIZE; j += PAGE_SIZE) {
			if (!mmu_frame_test(a + j)) {
				hole = j + PAGE_SIZE;
			} else {
				hole = 0;
				a += j;
				break;
			}
		}
		if (hole != 0)
			return hole >> PAGE_SHIFT;
	}
	
	panic("failed to alloc %d frames", n);
	return (uint32_t) -1;
}

static uint32_t mmu_first_frame(void) {
	for (uint32_t i = 0; i < INDEX_FROM_BIT(nframes); ++i) {
		if (frames[i] != (uint64_t) -1) {
			for (uint32_t b = 0; b < 64; ++b) {
				if (!(frames[i] & (0x1 << b)))
					return (i << PAGE_SHIFT) + b;
			}
		}
	}
	panic("failed to alloc a frame", 1);
	return (uint32_t) -1;
}

static spinlock_t frame_alloc_lock = { 0 };

#define __pagetable __attribute((aligned(PAGE_SIZE))) = {0}
static pml_t pml4[512] __pagetable;
static pml_t pdpt_ident[512] __pagetable;
static pml_t pdpt_kernl[512] __pagetable;
static pml_t pd_2mb[2][512] __pagetable;

extern symbol_t _kernel_vma;
extern symbol_t _end;

void mmu_init(size_t mem_amount) {
	spinlock_init(frame_alloc_lock);
	
	pml4[0  ].raw = (uint64_t)&pdpt_ident | PAGE_KERNEL_FLAG;
	pml4[511].raw = (uint64_t)&pdpt_kernl | PAGE_KERNEL_FLAG;
	
	pdpt_ident[0  ].raw = (uint64_t)&pd_2mb | PAGE_KERNEL_FLAG;
	pdpt_kernl[510].raw = (uint64_t)&pd_2mb | PAGE_KERNEL_FLAG
	
	for (uint32_t i = 0; i < 512; ++i) {
		pd_2mb[i] = (i * LARGE_PAGE_SIZE) | PAGE_SIZE_FLAG | PAGE_KERNEL_FLAG;
	}
		
	nframes = mem_amount >> 12;
}
