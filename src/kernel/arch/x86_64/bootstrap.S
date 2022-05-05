.section .boot,"ax",@progbits
.code32

.extern _stack_bottom_phys

.global _start
.align 4
_start:
	mov $_stack_bottom_phys, %esp
	push $0              
	push %ebx                    # Push Mutliboot Info Structure Location
	push $0              
	push %eax                    # Push Multiboot Magic Number

	mov $_pml4, %eax             # Load CR3 with the PLM4
	mov %eax, %cr3

	mov %cr4, %eax
	or $(1 << 7 | 1 << 5), %eax  # PGE, PAE
	mov %eax, %cr4

	mov $0xC0000080, %ecx        # Read from the EFER MSR
	rdmsr
	or $(1 << 11 | 1 << 8 | 1 << 0), %eax # Set LME and NXE bits and SCE bits
	wrmsr

	mov %cr0, %eax               # Enable Paging
	or $(1 << 31 | 1 << 16), %eax # Paging, WP Supervisor
	mov %eax, %cr0

	lgdt (_gdt_ptr)              # Long Mode GDT
	ljmp $0x8, $_long_mode       # Long Jump with code selector

# Page Boundary, Memory Info, Video Mode
.set HEADER_FLAGS, (1 << 0 | 1 << 1 | 1 << 2) 
# Multiboot Magic
.set HEADER_MAGIC, (0x1BADB002)
# Combine to make checksum
.set HEADER_CHECKSUM, -( HEADER_MAGIC + HEADER_FLAGS )

# We don't use A.OUT Kludge because it really is a Kludge, in order for it to
# load, .bss has to be progbits, when it should be nobits this bloats the file
# size and load time for no reason.
.align 4
_multiboot_hdr:
	.long HEADER_MAGIC
	.long HEADER_FLAGS
	.long HEADER_CHECKSUM
	.long 0 # These are zero     # header_addr
	.long 0 # because we aren't  # load_addr
	.long 0 # using A.OUT Kludge # load_end_addr
	.long 0                      # bss_end_addr
	.long 0                      # entry_addr
	.long 0                      # linear graphics type
	.long 1024                   # width
	.long 768                    # height
	.long 32                     # colour depth

.align 8
_gdt:
	.quad 0                            # Null (required first entry)
	.byte 0, 0, 0, 0, 0, 0x9A, 0xA0, 0 # code 0x8
	.byte 0, 0, 0, 0, 0, 0x92, 0x00, 0 # data 0x10
_gdt_end:

_gdt_ptr:
	.short _gdt_end - _gdt - 1   # The size of the table
	.quad _gdt                   # The base of the table

.align 4096
_pml4:
	.quad 0x0000000000102003     # pdpt_low
	.fill 510, 8, 0
	.quad 0x0000000000103003     # pdpt_high
_pdpt_low:
	.quad 0x0000000000000083     # 0gb virt (1gb sz)
	.fill 511, 8, 0
_pdpt_high:
	.fill 510, 8, 0
	.quad 0x0000000000000083     # -2gb virt (1gb sz)
	.fill 1, 8, 0

.code64
.extern arch_kentry
.extern _stack_bottom

.align 8
_long_mode:
	mov $0x10, %ax               # Set data selectors
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	pop %rdi                     # Magic Number
	pop %rsi                     # Multiboot Pointer
	mov $_stack_bottom, %rsp     # Set out higher half stack pointer

	call arch_kentry
