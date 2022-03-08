#include <hw/multiboot.h>
#include <kernel/arch.h>
#include <kernel/kprintf.h>
#include <kernel/vmm.h>
#include <utils/assert/panic.h>
#include <utils/base/defs.h>
#include <utils/mem.h>

#include <kernel/arch/x86_64/alloc.h>
#include <kernel/arch/x86_64/asm.h>
#include <kernel/arch/x86_64/context.h>
#include <kernel/arch/x86_64/mmu.h>
#include <kernel/arch/x86_64/pic.h>
#include <kernel/arch/x86_64/syscall.h>
#include <kernel/arch/x86_64/tables.h>

static size_t early_log_writter(uint8_t* buf, size_t len);

noreturn void arch_kentry(uint32_t mb_magic, struct multiboot_info* mb) {
	// Setup our early logging
	kprintf_writter = early_log_writter;

	// Validate and read our multiboot structures
	if (!multiboot_validate(mb_magic))
		panic$("Was not run by a multiboot loader as was expected\r\n");
	uint64_t mem_amount;
	multiboot_scan(mb, &mem_amount, null);

	alloc_init(mem_amount);
	vmm_init();
	gdt_init();
	idt_init();
	syscall_init();
	pic_init();

	arch_enable_interrupts();

	extern symbol_t _usermode;
	extern symbol_t _end;
	extern symbol_t _kernel_vma;

	vmm_set_flags(vmm_kernel_space(),
	              range$(vm_range_t,
	                     (vm_addr_t)_usermode + (vm_addr_t)_kernel_vma,
	                     align_up$((vm_addr_t)_end - (vm_addr_t)_usermode,
	                               MEM_PAGE_SIZE)),
	              MEM_WRITABLE | MEM_USER);

	context_t ctx = syscall_get_new_context();
	syscall_gs_base((uintptr_t)&ctx);
	sysret_enter_usermode(ctx.regs.rip, ctx.regs.rsp);

	panic$("This thread is not re-entrant, yet somehow we are here\r\n");
}

static size_t early_log_writter(uint8_t* buf, size_t len) {
	for (uint32_t i = 0; i < len; ++i)
		asm_outb(0x3F8, buf[i]); /* COM1 */
	return len;
}
