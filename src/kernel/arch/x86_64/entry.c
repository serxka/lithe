#include <kernel/kprintf.h>
#include <kernel/multiboot.h>
#include <kernel/vmm.h>
#include <lithe/base/attributes.h>
#include <lithe/base/defs.h>
#include <lithe/mem.h>

#include "alloc.h"
#include "asm.h"
#include "context.h"
#include "mmu.h"
#include "syscall.h"

static size_t early_log_writter(uint8_t* buf, size_t len) {
#define EARLY_LOG_PORT (0x3F8) /* COM1 */
	for (uint32_t i = 0; i < len; ++i)
		asm_outb(EARLY_LOG_PORT, buf[i]);
	return len;
}

static void early_log_init(void) {
	kprintf_writter = early_log_writter;
}

// The amount of memory in the system
static uintptr_t mem_amount = 0;
// Highest memory location of a multiboot module, after this all memory is free
// for use
static uintptr_t highest_address = 0;

void multiboot_scan(struct multiboot_info* mb) {
	// Get amount of memory in system
	if (mb->flags & MULTIBOOT_INFO_MEMORY)
		mem_amount = (uintptr_t)mb->mem_upper * 0x400 + 0x100000;

	// Find the highest location of multiboot modules
	if (mb->flags & MULTIBOOT_INFO_MODS) {
		mboot_mods_t* mods = (mboot_mods_t*)(uintptr_t)mb->mods_addr;
		for (uint32_t i = 0; i < mb->mods_count; ++i) {
			uintptr_t addr = mods[i].mod_start + mods[i].mod_end;
			if (addr > highest_address)
				highest_address = addr;
		}
	}
}

// Because at this point header files for a single function is dumb
void gdt_init(void);
void idt_init(void);
void pic_init(void);

void kentry(uint32_t mb_magic, struct multiboot_info* mb) {
	UNUSED(mb_magic);

	early_log_init();
	multiboot_scan(mb);

	alloc_init(mem_amount);
	vmm_init();
	gdt_init();
	idt_init();
	syscall_init();
	pic_init();

	asm_sti();

	extern symbol_t _usermode;
	extern symbol_t _end;
	extern symbol_t _kernel_vma;

	vmm_set_flags(vmm_kernel_space(),
	              range$(vm_range,
	                     (vm_addr)_usermode + (vm_addr)_kernel_vma,
	                     ALIGN_UP((vm_addr)_end - (vm_addr)_usermode,
	                              MEM_PAGE_SIZE)),
	              MEM_WRITABLE | MEM_USER);

	context_t ctx = syscall_get_new_context();
	syscall_gs_base((uintptr_t)&ctx);
	sysret_enter_usermode(ctx.regs.rip, ctx.regs.rsp);

	// // Test page fault handler
	*(volatile uint8_t*)0xdeadbeef;
loop:
	halt();
	goto loop;
}
