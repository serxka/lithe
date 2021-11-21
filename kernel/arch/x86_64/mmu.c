#include <string.h>

#include <kernel/common.h>
#include <kernel/kprintf.h>
#include <kernel/spinlock.h>

#include "mmu.h"

#define PAGE_SHIFT (12)
#define PAGE_SIZE (0x1 << PAGE_SHIFT)
#define PAGE_MASK 0x1FFF

#define LARGE_PAGE_SIZE (0x1 << 21) /*2MiB*/

#define PAGE_KERNEL_FLAG 0x03
#define PAGE_SIZE_FLAG 0x80

#define INDEX_FROM_BIT(x) ((x) >> 6)
#define OFFSET_FROM_BIT(x) ((x)&0x3F)

u64 *frames;
u32 nframes;

static void mmu_frame_set(uintptr_t addr)
{
	u32 frame = addr >> PAGE_SHIFT;
	u32 idx = INDEX_FROM_BIT(frame);
	u32 off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

static void mmu_frame_clear(uintptr_t addr)
{
	u32 frame = addr >> PAGE_SHIFT;
	u32 idx = INDEX_FROM_BIT(frame);
	u32 off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}

static bool mmu_frame_test(uintptr_t addr)
{
	u32 frame = addr >> PAGE_SHIFT;
	u32 idx = INDEX_FROM_BIT(frame);
	u32 off = OFFSET_FROM_BIT(frame);
	return frames[idx] & (0x1 << off);
}

static u32 mmu_first_frame(void)
{
	for (u32 i = 0; i < INDEX_FROM_BIT(nframes); ++i) {
		if (frames[i] != (u64)-1) {
			for (u32 b = 0; b < 64; ++b) {
				if (!(frames[i] & (0x1 << b)))
					return (i + b) << PAGE_SHIFT;
			}
		}
	}
	panic("failed to alloc a frame", 1);
	return (u32)-1;
}

extern symbol_t _kernel_vma;
extern symbol_t _end;

#define __pagetable __attribute((aligned(PAGE_SIZE)))
static pml_t pml4[512] __pagetable;
static pml_t pdpt_ident[512] __pagetable;
static pml_t pdpt_kernl[512] __pagetable;
static pml_t pd_2mb[2][512] __pagetable;

static u8 *heap_start;
static spinlock_t frame_alloc_lock = {0};
static phys_t current_tree;

phys_t mmu_kernel2phys(logi_t addr)
{
	return addr - (phys_t)_kernel_vma;
}

void mmu_init(size_t mem_amount)
{
	// Initialise our memory locks
	spinlock_init(frame_alloc_lock);

	// Map our kernel pml4 to 0gb (ident) and -512gb (kerenl)
	pml4[0].raw = mmu_kernel2phys((uintptr_t)pdpt_ident) | PAGE_KERNEL_FLAG;
	pml4[511].raw =
		mmu_kernel2phys((uintptr_t)pdpt_kernl) | PAGE_KERNEL_FLAG;

	// Start our identity mapping at 0gb
	pdpt_ident[0].raw =
		mmu_kernel2phys((uintptr_t)&pd_2mb[0]) | PAGE_KERNEL_FLAG;
	pdpt_ident[1].raw =
		mmu_kernel2phys((uintptr_t)&pd_2mb[1]) | PAGE_KERNEL_FLAG;
	// and our kernel at -2gb (kernel memory model)
	pdpt_kernl[510].raw =
		mmu_kernel2phys((uintptr_t)&pd_2mb[0]) | PAGE_KERNEL_FLAG;
	pdpt_kernl[511].raw =
		mmu_kernel2phys((uintptr_t)&pd_2mb[1]) | PAGE_KERNEL_FLAG;

	// Go through and map all of our 2mb pages from 0-2gb
	for (u32 i = 0; i < 512; ++i) {
		pd_2mb[0][i].raw = (0x00000000 + i * LARGE_PAGE_SIZE) |
				   PAGE_SIZE_FLAG | PAGE_KERNEL_FLAG;
		pd_2mb[1][i].raw = (0x40000000 + i * LARGE_PAGE_SIZE) |
				   PAGE_SIZE_FLAG | PAGE_KERNEL_FLAG;
	}

	// Load a pointer to our plm4 into cr3
	__asm__ __volatile__("movq %0, %%cr3"
			     :
			     : "r"(mmu_kernel2phys((uintptr_t)pml4)));

	// Set the amount pages of usable memory in the system
	nframes = mem_amount >> PAGE_SHIFT;
	// Get the amount of bytes out bitmap takes up
	size_t frame_bytes = INDEX_FROM_BIT(nframes) * 8;
	// Round up (8 bytes -- u64) and set out frame pointer
	frames = (u64 *)(((u64)_end + (0x8 - 1)) & ~0x7);
	// Zero our bitmap
	memset(frames, 0, frame_bytes);

	// The end of our kernel and free memory after, rounding up
	u64 kernel_end = ((u64)_end + frame_bytes + (PAGE_SIZE - 1)) & ~0xFFF;

	// Set all the pages that we need to fit our kernel
	for (uintptr_t i = 0; i < kernel_end; i += PAGE_SIZE)
		mmu_frame_set(i);

	kprintf("%x, %x\r\n", mmu_first_frame(), mmu_first_frame());

	heap_start = (u8 *)kernel_end;
}

void mmu_switch_tree(phys_t addr)
{
	current_tree = addr;
	__asm__ __volatile__("mov %0, %%cr3" ::"r"(current_tree));
}
