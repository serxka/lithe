#include <kernel/common.h>
#include <kernel/kprintf.h>
#include <kernel/string.h>

#include "irq.h"

typedef struct idt_entry_s {
	u16 offset_lower;
	u16 selector;
	u8 ist;
	u8 type_attr;
	u16 offset_middle;
	u32 offset_upper;
	u32 _res0;
} PACKED idt_entry_t;

static struct {
	u16 limit;
	u64 offset;
} PACKED idt_ptr;
static idt_entry_t idt[256];

extern void page_fault_isr(void);

static void idt_set_gate(u8 idx, void (*handler)(void), u16 segment, u8 flags,
			 u8 ist) {
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
