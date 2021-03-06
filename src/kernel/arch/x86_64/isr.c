#include <kernel/arch.h>
#include <kernel/kprintf.h>
#include <utils/base/defs.h>

#include <kernel/arch/x86_64/asm.h>
#include <kernel/arch/x86_64/isr.h>

static void dump_regs(interrupt_frame_err_t* frame) {
	kprintf("Registers:\r\n"
	        "\trip: %x:[%x] flags: %x\r\n"
	        "\trax: %x rbx: %x rcx: %x rdx: %x\r\n"
	        "\trsi: %x rdi: %x rbp: %x rsp: %x\r\n",
	        frame->cs, frame->rip, frame->rflags, frame->rax, frame->rbx,
	        frame->rcx, frame->rdx, frame->rsi, frame->rdi, frame->rbp,
	        frame->rsp);
}

void double_fault_handler(interrupt_frame_err_t* frame) {
	kprintf("----------------\r\n[ KENREL PANIC ]\r\n----------------\r\n");
	kprintf("double fault\r\n");
	dump_regs(frame);

	arch_halt();
}

void gp_fault_handler(interrupt_frame_err_t* frame) {
	kprintf("----------------\r\n[ KENREL PANIC ]\r\n----------------\r\n");
	kprintf("general protection fault\r\n");
	dump_regs(frame);

	arch_halt();
}

void page_fault_handler(interrupt_frame_err_t* frame) {
	uintptr_t cr2 = asm_read_cr2();
	kprintf("----------------\r\n[ KENREL PANIC ]\r\n----------------\r\n");
	kprintf("PAGE FAULT at address [%x] code: (%x)\r\n", cr2,
	        frame->err_code);
	dump_regs(frame);

	arch_halt();
}
