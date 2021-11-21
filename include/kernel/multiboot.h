#ifndef _KERNEL_MULTIBOOT_H_
#define _KERNEL_MULTIBOOT_H_

#include <kernel/common.h>

#define MULTIBOOT_LOADER_MAGIC 0x2BADB002

#define MULTIBOOT_INFO_MEMORY 0x01
#define MULTIBOOT_INFO_BOOTDEV 0x02
#define MULTIBOOT_INFO_CMDLINE 0x04
#define MULTIBOOT_INFO_MODS 0x08

struct multiboot_aout_table {
	u32 tabsize;
	u32 strsize;
	u32 addr;
	u32 _reserved;
};

struct multiboot_info {
	u32 flags;

	u32 mem_lower;
	u32 mem_upper;

	u32 boot_device;

	u32 cmdline;

	u32 mods_count;
	u32 mods_addr;

	union {
		struct multiboot_aout_table aout_sym;
		// ...
	} u;

	u32 mmap_length;
	u32 mmap_addr;

	u32 drives_length;
	u32 drive_addr;

	u32 config_table;

	u32 boot_loader_name;

	u32 apm_table;

	u32 vbe_control_info;
	u32 vbe_mode_info;
	u16 vbe_mode;
	u16 vbe_interface_seg;
	u16 vbe_interface_off;
	u16 vbe_interface_len;

	u64 framebuf_addr;
	u32 framebuf_pitch;
	u32 framebuf_width;
	u32 framebuf_height;
	u32 framebuf_type;
};

struct multiboot_mmap_entry {
	u32 size;
	u64 addr;
	u64 len;

#define MULTIBOOT_MEM_AVALIABLE 1
#define MULTIBOOT_MEM_RESERVE 2
#define MULTIBOOT_MEM_ACPI_RECLAIM 3
#define MULTIBOOT_MEM_NVS 4
#define MULTIBOOT_MEM_BADRAM 5
	u32 type;
} __attribute__((packed));
typedef struct multiboot_mmap_entry mboot_mmap_t;

struct multiboot_mod_list {
	u32 mod_start;
	u32 mod_end;
	u32 cmdline;
	u32 _pad;
};
typedef struct multiboot_mod_list mboot_mods_t;

#endif // _KERNEL_MULTIBOOT_H_
