#include <kernel/kprintf.h>
#include <lithe/base/defs.h>
#include <lithe/mem.h>
#include <lithe/sync/spinlock.h>

#include "alloc.h"
#include "mmu.h"

extern symbol_t _kernel_vma;
extern symbol_t _end;

// Page frame allocator

#define INDEX_FROM_BIT(x) ((x) >> 6)
#define OFFSET_FROM_BIT(x) ((x)&0x3F)

static spinlock frame_alloc_lock = {0};
static uint64_t* frames;
static uint32_t nframes;
static uint8_t* heap_start;

void alloc_init(uint64_t mem_available) {
	// Initialise our memory locks
	spinlock_init(frame_alloc_lock);

	// Set the amount pages of usable memory in the system
	nframes = mem_available >> MEM_PAGE_SHIFT;
	// Get the amount of bytes out bitmap takes up
	size_t frame_bytes = INDEX_FROM_BIT(nframes) * 8;
	// Round up (8 bytes -- uint64_t) and set our frame pointer
	frames = (uint64_t*)(ALIGN_UP((uint64_t)_end, 0x8)
	                     + (uint64_t)_kernel_vma);
	// Zero our bitmap
	memset(frames, 0, frame_bytes);

	// The end of our kernel and free memory after, rounding up
	uint64_t kernel_end =
	        ALIGN_UP((uint64_t)_end + frame_bytes, MEM_PAGE_SIZE);

	// Set all the pages that we need to fit our kernel
	for (uint64_t i = 0; i < kernel_end; i += 0x1000)
		alloc_frame_set(i);

	heap_start = (uint8_t*)kernel_end;
}

void alloc_frame_set(uint64_t addr) {
	uint32_t frame = addr >> MEM_PAGE_SHIFT;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

void alloc_frame_clear(uint64_t addr) {
	uint32_t frame = addr >> MEM_PAGE_SHIFT;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}

bool alloc_frame_test(uint64_t addr) {
	uint32_t frame = addr >> MEM_PAGE_SHIFT;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	return frames[idx] & (0x1 << off);
}

alloc_result alloc_first_frame(void) {
	for (uint32_t i = 0; i < INDEX_FROM_BIT(nframes); ++i) {
		// Quick check whole 64 pages using mask
		if (frames[i] == ~(uint64_t)0)
			continue;
		// otherwise look through byte of a free page
		for (uint32_t b = 0; b < 64; ++b) {
			// Skip is page is set
			if (frames[i] & (0x1 << b))
				continue;
			uintptr_t addr = (i * 64 + b) << MEM_PAGE_SHIFT;
			alloc_frame_set(addr);
			return OK(alloc_result, addr);
		}
	}
	return ERR(alloc_result, ERR_OUT_OF_MEMORY);
}
