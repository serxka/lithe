#include <kernel/generic.h>
#include <kernel/types.h>
#include <kernel/misc/kprintf.h>

#include <kernel/arch/x86_64/multiboot.h>
#include <kernel/arch/x86_64/port.h>

static size_t early_log_writter(uint8_t *buf, size_t len) {
	#define EARLY_LOG_PORT (0x3F8) /* COM1 */
	for (uint32_t i = 0; i < len; ++i)
		port_outb(EARLY_LOG_PORT, buf[i]);
	return len;
}

static void early_log_init(void) {
	kprintf_writter = &early_log_writter;
}

// The amount of memory in the system
static size_t mem_amount = 0;
// Highest memory location of a multiboot module, after this all memory is free for use
static uintptr_t highest_address = 0;
void multiboot_scan(struct multiboot_info *mb) {
	if (mb->flags & MULTIBOOT_INFO_MEMORY)
		mem_amount = (uintptr_t)mb->mem_upper * 0x400 + 0x100000;
	
	if (mb->flags & MULTIBOOT_INFO_MODS) {
		mboot_mods_t *mods = (mboot_mods_t*)(uintptr_t)mb->mods_addr;
		for (uint32_t i = 0; i < mb->mods_count; ++i) {
			uintptr_t addr = mods[i].mod_start + mods[i].mod_end;
			if (addr > highest_address)
				highest_address = addr;
		}
	}
}

int kentry(uint32_t mb_magic, struct multiboot_info *mb) {
	early_log_init();
	
	multiboot_scan(mb);
	// kprintf("highest loc: %x\r\nmem amount: %d", highest_address, mem_amount / 1024 / 1024);

	return generic_main();
}
