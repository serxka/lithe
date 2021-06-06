#include <kernel/types.h>
#include <kernel/kprintf.h>

#include "include/mmu.h"

typedef struct gdt_entry_s {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t flags_limit;
	uint8_t base_high;
}__attribute__ ((packed)) gdt_entry_t;

typedef struct gdt_ptr_s {
	uint16_t limit;
	uint64_t base;
}__attribute__((packed)) gdt_ptr_t;

typedef struct tss_entry_s {
	uint32_t _res0;
	uint64_t rsp[3];
	uint32_t _res1;
	uint64_t ist[7];
	uint64_t _res2;
	uint16_t _res3;
	uint16_t iomap_addr;
}__attribute__((packed)) tss_entry_t;

struct gdt_tss {
	gdt_entry_t entries[6];
	struct {
		uint64_t base_upper;
		uint64_t _res0;
	}__attribute__((packed)) tss_extra;
	struct {
		uint16_t limit;
		uint64_t offset;
	}__attribute__((packed)) ptr;
	tss_entry_t tss;
}__attribute__((packed, aligned(0x10)));

// Our last GDT entry is our TSS one, this needs another 64bits afterwards (Vol. 3A, 7.2.3)
static struct gdt_tss gdt = {
	.entries = {
		{0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00}, // NULL
		{0x0000, 0x0000, 0x00, 0x9A, 0xAF, 0x00}, // Kernel Code
		{0x0000, 0x0000, 0x00, 0x92, 0xA0, 0x00}, // Kernel Data
		{0xFFFF, 0x0000, 0x00, 0xFA, 0xAF, 0x00}, // User Code
		{0xFFFF, 0x0000, 0x00, 0xF2, 0xAF, 0x00}, // User Data
		{0x0000, 0x0000, 0x00, 0xE9, 0x00, 0x00}, // TSS Segement
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
	gdt.tss.rsp[0] = (uint64_t)_stack_bottom_phys;
	
	__asm__ __volatile__ (
		"lgdt (%0)\n"
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
		"mov $0x2b, %%ax\n" // TSS Segement user priv
		"ltr %%ax\n"
		: : "r"((uintptr_t)&gdt.ptr)
	);
}
