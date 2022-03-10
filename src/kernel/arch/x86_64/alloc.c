#include <kernel/kprintf.h>
#include <utils/base/defs.h>
#include <utils/mem.h>
#include <utils/sync/spinlock.h>
#include <utils/assert/assert.h>

#include <kernel/arch/x86_64/alloc.h>
#include <kernel/arch/x86_64/mmu.h>

extern symbol_t _kernel_vma;
extern symbol_t _end;

// Page frame allocator

#define INDEX_FROM_BIT(x) ((x) >> 6)
#define OFFSET_FROM_BIT(x) ((x)&0x3F)

static spinlock_t frame_alloc_lock = {0};
static uint64_t* frames;
static uint32_t nframes;

static void alloc_frame_set(uint64_t addr) {
	uint32_t frame = addr >> MEM_PAGE_SHIFT;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

static void alloc_frame_clear(uint64_t addr) {
	uint32_t frame = addr >> MEM_PAGE_SHIFT;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}

static bool alloc_frame_test(uint64_t addr) {
	uint32_t frame = addr >> MEM_PAGE_SHIFT;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	return frames[idx] & (0x1 << off);
}

void alloc_init(uint64_t mem_available) {
	// Initialise our memory locks
	spinlock_init$(frame_alloc_lock);

	// Set the amount pages of usable memory in the system
	nframes = mem_available >> MEM_PAGE_SHIFT;
	// Get the amount of bytes out bitmap takes up
	size_t frame_bytes = INDEX_FROM_BIT(nframes) * 8;
	// Round up (8 bytes -- uint64_t) and set our frame pointer
	frames = (uint64_t*)(align_up$((uint64_t)_end, 0x8)
	                     + (uint64_t)_kernel_vma);
	// Zero our bitmap
	memset(frames, 0, frame_bytes);

	// The end of our kernel and free memory after, rounding up
	uint64_t kernel_end =
	        align_up$((uint64_t)_end + frame_bytes, MEM_PAGE_SIZE);

	// Set all the pages that we need to fit our kernel
	for (uint64_t i = 0; i < kernel_end; i += MEM_PAGE_SIZE)
		alloc_frame_set(i);
}

static alloc_result_t alloc_first_frame(void) {
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
			return OK(alloc_result_t, addr);
		}
	}
	return ERR(alloc_result_t, ERR_OUT_OF_MEMORY);
}

alloc_result_t alloc_pages(bool physical, size_t page_count) {
	if (page_count == 0)
		return ERR(alloc_result_t, ERR_BAD_PARAMETERS);

	// Try the first page of allocated memory
	uintptr_t base = try$(alloc_result_t, alloc_first_frame());
	for (size_t i = 1; i < page_count; ++i) {
		alloc_result_t res = alloc_first_frame();
		if (res.fail) {
			// Free any memory from the failed allocation
			alloc_free_pages(physical, base, i);
			return res;
		}
	}

	return OK(alloc_result_t,
	          physical ? base : (base + (uintptr_t)_kernel_vma));
}

void alloc_free_pages(bool physical, uintptr_t addr, size_t page_count) {
	addr = physical ? addr : (addr - (uintptr_t)_kernel_vma);
	for (size_t i = 0; i < page_count; ++i) {
		assert_true$(alloc_frame_test(addr + i * MEM_PAGE_SIZE));
		alloc_frame_clear(addr + i * MEM_PAGE_SIZE);
	}
}
