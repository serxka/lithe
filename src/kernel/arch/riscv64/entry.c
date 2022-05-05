#include <hw/sbi.h>
#include <kernel/arch.h>
#include <kernel/arch/riscv64/asm.h>
#include <kernel/kprintf.h>
#include <utils/assert/panic.h>
#include <utils/base/defs.h>

static size_t early_log_writter(uint8_t* buf, size_t len);

noreturn void arch_kentry(uint64_t hartid, void* dtb) {
	// Setup our early logging
	kprintf_writter = early_log_writter;

	kprintf("hello riscv! we are running on hart %d and our DTB address is "
	        "%x\r\n",
	        hartid, dtb);

	arch_halt();
}

static size_t early_log_writter(uint8_t* buf, size_t len) {
	for (uint32_t i = 0; i < len; ++i)
		sbi_console_putchar(buf[i]);
	return len;
}
