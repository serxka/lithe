.altmacro
.section .text
.code64

.macro pushaq
	push %rax
	push %rbx
	push %rcx
	push %rdx
	push %rsi
	push %rdi
	push %rbp
	push %r8
	push %r9
	push %r10
	push %r11
	push %r12
	push %r13
	push %r14
	push %r15
.endm

.macro popaq do_rax
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %rbp
	pop %rdi
	pop %rsi
	pop %rdx
	pop %rcx
	pop %rbx
	.ifc \do_rax,'do_rax'
		pop %rax
	.endif
.endm

.macro isr_stub isr handler
	.extern \handler
	.global \isr
	\isr:
		pushaq
		mov %rsp, %rdi
		call \handler
		popaq do_rax
		iretq
.endm

isr_stub double_fault_isr, double_fault_handler
isr_stub gp_fault_isr, gp_fault_handler
isr_stub page_fault_isr, page_fault_handler
isr_stub timer_irq, timer_handler

.extern syscall_handler
.align 4096
.global syscall_entry
syscall_entry:
	# We don't have to concern ourselves with ever calling swapgs whilst in
	# kernel-space as SYSCALL's are only ever called from user-space.
	swapgs             # swap from user GS to kernel GS
	mov %rsp, %gs:0x8  # save the current stack to the context structure
	mov %gs:0x0, %rsp  # use the kernel stack for syscalls

	sti                # now that the stack has been set, re-enable interrupts

	cld                # clear direction flag
	push %r11          # push saved rflags
	push %rcx          # push saved rip
	pushq %gs:0x8      # push saved rsp
	pushaq             # push all other registers

	mov %rsp, %rdi
	mov $0, %rbp

	callq syscall_handler

	popaq dont_rax     # pop everything but rax, stores return value

	cli                # no interrupts as we switch stacks
	mov %gs:0x8, %rsp  # move the user stack back in
	swapgs
	sysretq            # fully return to candy land

.global sysret_enter_usermode
sysret_enter_usermode:
	mov %rdi, %rcx
	mov %rsi, %rsp
	mov 0x202, %r11
	sysretq
