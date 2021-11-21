#include <kernel/common.h>
#include <kernel/kprintf.h>

#include "mmu.h"

typedef struct gdt_entry_s {
	u16 limit_low;
	u16 base_low;
	u8 base_middle;
	u8 access;
	u8 flags_limit;
	u8 base_high;
} PACKED gdt_entry_t;

typedef struct gdt_ptr_s {
	u16 limit;
	u64 base;
} PACKED gdt_ptr_t;

typedef struct tss_entry_s {
	u32 _res0;
	u64 rsp[3];
	u32 _res1;
	u64 ist[7];
	u64 _res2;
	u16 _res3;
	u16 iomap_addr;
} PACKED tss_entry_t;

struct gdt_tss {
	gdt_entry_t entries[6];
	struct {
		u64 base_upper;
		u64 _res0;
	} PACKED tss_extra;
	struct {
		u16 limit;
		u64 offset;
	} PACKED ptr;
	tss_entry_t tss;
} PACKED ALIGNED(0x10);

// Our last GDT entry is our TSS one, this needs another 64-bits afterwards
// (Vol. 3A, 7.2.3)
static struct gdt_tss gdt = {
	.entries =
		{
			{0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00}, // NULL
			{0x0000, 0x0000, 0x00, 0x9A, 0xA0, 0x00}, // Kernel Code
			{0x0000, 0x0000, 0x00, 0x92, 0xA0, 0x00}, // Kernel Data
			{0x0000, 0x0000, 0x00, 0xFA, 0xA0, 0x00}, // User Code
			{0x0000, 0x0000, 0x00, 0xF2, 0xA0, 0x00}, // User Data
			{0x0000, 0x0000, 0x00, 0xE9, 0x00, 0x00}, // TSS Segment
		},
	.tss_extra = {0, 0},
	.ptr = {0, 0},
	.tss = {0, {0, 0, 0}, 0, {0, 0, 0, 0, 0, 0, 0}, 0, 0, 0},
};

void gdt_init(void) {
	// Set our GDT pointer for asm(lgdt)
	gdt.ptr.limit = sizeof(gdt.entries) + sizeof(gdt.tss_extra) - 1;
	gdt.ptr.offset = (uintptr_t)&gdt.entries;

	// Set our TSS segment up
	uintptr_t tss_addr = (uintptr_t)&gdt.tss;
	gdt.entries[5].limit_low = sizeof(gdt.tss);
	gdt.entries[5].base_low = tss_addr & 0xFFFF;
	gdt.entries[5].base_middle = (tss_addr >> 16) & 0xFF;
	gdt.entries[5].base_high = (tss_addr >> 24) & 0xFF;
	gdt.tss_extra.base_upper = (tss_addr >> 32) & 0xFFFFFFFF;

	extern symbol_t _stack_bottom_phys;
	gdt.tss.rsp[0] = (u64)_stack_bottom_phys;

	__asm__ __volatile__("lgdt (%0)\n"
			     "pushq $0x8\n" // Kernel Code
			     "leaq +%=f(%%rip), %%rax\n"
			     "pushq %%rax\n"
			     "lretq\n"
			     "%=:\n"
			     "mov $0x10, %%ax\n" // Kernel Data
			     "mov %%ax, %%ds\n"
			     "mov %%ax, %%es\n"
			     "mov %%ax, %%fs\n"
			     "mov %%ax, %%gs\n"
			     "mov %%ax, %%ss\n"
			     "mov $0x2b, %%ax\n" // TSS Segment user privilege
			     "ltr %%ax\n"
			     :
			     : "r"((uintptr_t)&gdt.ptr));
}
