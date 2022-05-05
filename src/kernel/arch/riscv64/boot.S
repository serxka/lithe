.section .text
.global _start
_start:
	# Disable interrupts for S-mode
	csrci sstatus, (1 << 1)

	# Set our stack pointer
	la sp, _stack
	mv fp, zero

	# a0 HART ID
	# a1 DTB
	tail arch_kentry
	unimp
