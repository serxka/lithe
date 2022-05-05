#include <kernel/arch.h>
#include <utils/base/defs.h>

#include <kernel/arch/riscv64/asm.h>

void arch_enable_interrupts(void) {
	asm_set_bits_sstatus(SSTATUS_SIE);
}

void arch_disable_interrupts(void) {
	asm_clear_bits_sstatus(SSTATUS_SIE);
}

noreturn void arch_halt(void) {
loop:
	asm_clear_bits_sstatus(SSTATUS_SIE);
	asm_wfi();
	goto loop;
}
