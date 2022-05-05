.section .text

.align 2
.global trap_vector
trap_vector:
	# Swap `s1` and `sscratch`
	csrrw s1, sscratch, s1

	# Store all registers
	sd ra, 0x00(s1)
	# ...

	# Save `sstatus`
	csrr t0, sstatus
	sd t0, 0x98(s1)
	# Save `sepc`
	csrr t0, sepc
	sd t0, 0xA0(s1)

	# Check if usermode was interrupted
	# switch stack, etc


	mv a0, s1
	#jal trap_handler


	sret
