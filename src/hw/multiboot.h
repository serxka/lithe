#pragma once

#include <utils/base/attributes.h>
#include <utils/base/defs.h>

#define MULTIBOOT_LOADER_MAGIC 0x2BADB002

#define MULTIBOOT_INFO_MEMORY 0x01
#define MULTIBOOT_INFO_BOOTDEV 0x02
#define MULTIBOOT_INFO_CMDLINE 0x04
#define MULTIBOOT_INFO_MODS 0x08

struct multiboot_aout_table {
	uint32_t tabsize;
	uint32_t strsize;
	uint32_t addr;
	uint32_t _reserved;
};

struct multiboot_info {
	uint32_t flags;

	uint32_t mem_lower;
	uint32_t mem_upper;

	uint32_t boot_device;

	uint32_t cmdline;

	uint32_t mods_count;
	uint32_t mods_addr;

	union {
		struct multiboot_aout_table aout_sym;
		// ...
	} u;

	uint32_t mmap_length;
	uint32_t mmap_addr;

	uint32_t drives_length;
	uint32_t drive_addr;

	uint32_t config_table;

	uint32_t boot_loader_name;

	uint32_t apm_table;

	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;

	uint64_t framebuf_addr;
	uint32_t framebuf_pitch;
	uint32_t framebuf_width;
	uint32_t framebuf_height;
	uint32_t framebuf_type;
};

struct multiboot_mmap_entry {
	uint32_t size;
	uint64_t addr;
	uint64_t len;

#define MULTIBOOT_MEM_AVALIABLE 1
#define MULTIBOOT_MEM_RESERVE 2
#define MULTIBOOT_MEM_ACPI_RECLAIM 3
#define MULTIBOOT_MEM_NVS 4
#define MULTIBOOT_MEM_BADRAM 5
	uint32_t type;
} PACKED;
typedef struct multiboot_mmap_entry mboot_mmap_t;

struct multiboot_mod_list {
	uint32_t mod_start;
	uint32_t mod_end;
	uint32_t cmdline;
	uint32_t _pad;
};
typedef struct multiboot_mod_list mboot_mods_t;

static bool multiboot_validate(uint32_t magic) {
	return magic == MULTIBOOT_LOADER_MAGIC;
}
static void multiboot_scan(struct multiboot_info* mb, uint64_t* mem_amount,
                           uint64_t* mem_highest_free) {
	// Get amount of memory in system
	if (mb->flags & MULTIBOOT_INFO_MEMORY && mem_amount != null)
		*mem_amount = (uint64_t)mb->mem_upper * 1024 + 0x100000;

	// Find the highest location of multiboot modules
	if (mb->flags & MULTIBOOT_INFO_MODS && mem_highest_free != null) {
		mboot_mods_t* mods = (mboot_mods_t*)(uint64_t)mb->mods_addr;
		for (uint32_t i = 0; i < mb->mods_count; ++i) {
			uint64_t addr = mods[i].mod_start + mods[i].mod_end;
			if (addr > *mem_highest_free)
				*mem_highest_free = addr;
		}
	}
}
