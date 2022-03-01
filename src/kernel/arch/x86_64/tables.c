#include <kernel/kprintf.h>
#include <lithe/base/attributes.h>
#include <lithe/base/defs.h>
#include <lithe/mem.h>

#include "irq.h"

// IDT

typedef struct {
	uint16_t offset_lower;
	uint16_t selector;
	uint8_t ist;
	uint8_t type_attr;
	uint16_t offset_middle;
	uint32_t offset_upper;
	uint32_t _res0;
} PACKED idt_entry_t;

static struct {
	uint16_t limit;
	uint64_t offset;
} PACKED idt_ptr;
static idt_entry_t idt[256];

extern void page_fault_isr(void);

static void idt_set_gate(uint8_t idx, void (*handler)(void), uint16_t segment,
                         uint8_t flags, uint8_t ist) {
	uintptr_t addr = (uintptr_t)handler;
	idt[idx].offset_lower = addr & 0xFFFF;
	idt[idx].offset_middle = (addr >> 16) & 0xFFFF;
	idt[idx].offset_upper = (addr >> 32) & 0xFFFFFFFF;
	idt[idx].selector = segment;
	idt[idx].ist = ist;
	idt[idx].type_attr = flags;
	idt[idx]._res0 = 0;
}

void idt_init(void) {
	// Set our IDT pointer, address is linear
	idt_ptr.limit = sizeof(idt) - 1;
	idt_ptr.offset = (uintptr_t)&idt;

	// Zero out IDT
	memset(idt, 0, sizeof(idt));

	idt_set_gate(14, page_fault_isr, 0x08, 0x8E, 0);

	// Load our IDT pointer into IDTR
	__asm__ __volatile__("lidt (%0)" : : "r"((uintptr_t)&idt_ptr));
}

static void dump_regs(interrupt_frame_err_t* frame) {
	kprintf("Registers:\r\n"
	        "\trip: %x:[%x] flags: %x\r\n"
	        "\trax: %x rbx: %x rcx: %x rdx: %x\r\n"
	        "\trsi: %x rdi: %x rbp: %x rsp: %x\r\n",
	        frame->cs, frame->rip, frame->rflags, frame->rax, frame->rbx,
	        frame->rcx, frame->rdx, frame->rsi, frame->rdi, frame->rbp,
	        frame->rsp);
}

void page_fault_handler(interrupt_frame_err_t* frame) {
	uintptr_t cr2;
	__asm__ __volatile__("mov %%cr2, %0" : "=r"(cr2) :);
	kprintf("----------------\r\n[ KENREL PANIC ]\r\n----------------\r\n");
	kprintf("PAGE FAULT at address [%x]\r\n", cr2);
	dump_regs(frame);
loop:
	__asm__ __volatile__("cli");
	__asm__ __volatile__("hlt");
	goto loop;
}

// GDT

typedef struct {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t flags_limit;
	uint8_t base_high;
} PACKED gdt_entry_t;

typedef struct {
	uint16_t limit;
	uint64_t base;
} PACKED gdt_ptr_t;

typedef struct {
	uint32_t _res0;
	uint64_t rsp[3];
	uint32_t _res1;
	uint64_t ist[7];
	uint64_t _res2;
	uint16_t _res3;
	uint16_t iomap_addr;
} PACKED tss_entry_t;

struct gdt_tss {
	gdt_entry_t entries[6];
	struct {
		uint64_t base_upper;
		uint64_t _res0;
	} PACKED tss_extra;
	struct {
		uint16_t limit;
		uint64_t offset;
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
	gdt.tss.rsp[0] = (uint64_t)_stack_bottom_phys;

	__asm__ __volatile__(
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
	        "mov $0x2b, %%ax\n" // TSS Segment user privilege
	        "ltr %%ax\n"
	        :
	        : "r"((uintptr_t)&gdt.ptr));
}
