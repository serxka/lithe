#include <kernel/arch.h>
#include <utils/base/defs.h>

#include <kernel/arch/x86_64/asm.h>

void arch_enable_interrupts(void) {
	asm_sti();
}

void arch_disable_interrupts(void) {
	asm_cli();
}

noreturn void arch_halt(void) {
loop:
	asm_cli();
	asm_hlt();
	goto loop;
}
