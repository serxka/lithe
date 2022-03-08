#include <kernel/kprintf.h>
#include <utils/base/defs.h>

#include <kernel/arch/x86_64/asm.h>
#include <kernel/arch/x86_64/context.h>

void syscall_entry(void);

void syscall_init(void) {
	asm_write_msr(MSR_STAR,
	              ((uint64_t)0x8 << 32) | (((uint64_t)0x18 | 0x3) << 48));
	asm_write_msr(MSR_LSTAR, (uint64_t)syscall_entry);
	asm_write_msr(MSR_FMASK, 0xffffffff);
}

void syscall_gs_base(uintptr_t addr) {
	asm_write_msr(MSR_GS_BASE, 0);
	asm_write_msr(MSR_KERNEL_GS_BASE, addr);
}

int syscall_handler(regs_t const* regs) {
	kprintf("got system call %x\r\n", regs->rax);

	return 0;
}

uint64_t __attribute__((section(".usermode_bss"))) ALIGNED(16) user_stack[32];
noreturn void __attribute__((section(".usermode"))) user_function(void) {
	// Make another system call
	__asm__ __volatile__("syscall" : : "a"(0x69));

	// Test the page fault handler
	*(volatile uint8_t*)0xdeadbeef;

	for (;;)
		;
}

context_t syscall_get_new_context(void) {
	static uint64_t ALIGNED(16) kernel_stack[2048];

	context_t ctx =
	        (context_t){.user_stack = (uintptr_t)(user_stack + 31),
	                    .kernel_stack = (uintptr_t)(kernel_stack + 2047),
	                    .regs = {0}};
	ctx.regs.rip = (uint64_t)user_function;
	ctx.regs.rsp = (uint64_t)ctx.user_stack;
	ctx.regs.rflags = 0x202;
	return ctx;
}
