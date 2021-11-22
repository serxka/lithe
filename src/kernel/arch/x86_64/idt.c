#include <kernel/kprintf.h>
#include <lithe/base/attributes.h>
#include <lithe/base/defs.h>
#include <lithe/mem.h>

#include "irq.h"

typedef struct idt_entry_s {
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

static void dump_regs(interrupt_frame_err_t *frame) {
	kprintf("Registers:\r\n"
		"\trip: %x:[%x] flags: %x\r\n"
		"\trax: %x rbx: %x rcx: %x rdx: %x\r\n"
		"\trsi: %x rdi: %x rbp: %x rsp: %x\r\n",
		frame->cs, frame->rip, frame->rflags, frame->rax, frame->rbx,
		frame->rcx, frame->rdx, frame->rsi, frame->rdi, frame->rbp,
		frame->rsp);
}

void page_fault_handler(interrupt_frame_err_t *frame) {
	uintptr_t cr2;
	__asm__ __volatile__("mov %%cr2, %0" : "=r"(cr2) :);
	dump_regs(frame);
	panic("PAGE FAULT at address [%x]", cr2);
}
