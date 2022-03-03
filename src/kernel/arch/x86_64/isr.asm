section .text
[bits 64]

%macro pushaq 0
	push rax
	push rbx
	push rcx
	push rdx
	push rsi
	push rdi
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popaq 1
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx
	%if %0 == rax
		pop rax
	%endif
%endmacro

%macro isr_stub 2
[extern %2]
[global %1]
%1:
	pushaq
	mov rdi, rsp
	call %2
	popaq rax
	iretq
%endmacro

isr_stub double_fault_isr, double_fault_handler
isr_stub gp_fault_isr, gp_fault_handler
isr_stub page_fault_isr, page_fault_handler
isr_stub timer_irq, timer_handler

[extern syscall_handler]
align 4096
[global syscall_entry]
syscall_entry:
	; We don't have to concern ourselves with ever calling swapgs whilst in
	; kernel-space as SYSCALL's are only ever called from user-space.
	swapgs             ; swap from user GS to kernel GS
	mov [gs:0x8], rsp  ; save the current stack to the context structure
	mov rsp, [gs:0x0]  ; use the kernel stack for syscalls

	sti             ; now that the stack has been set, re-enable interrupts

	cld                ; clear direction flag
	push r11           ; push saved rflags
	push rcx           ; push saved rip
	push qword [gs:0x8]; push saved rsp
	pushaq             ; push all other registers
	
	mov rdi, rsp
	mov rbp, 0
	
	call syscall_handler
	
	popaq none         ; pop everything but rax, stores return value

	cli                ; no interrupts as we switch stacks
	mov rsp, [gs:0x8]  ; move the user stack back in
	swapgs
	o64 sysret         ; fully return to candy land

[global sysret_enter_usermode]
sysret_enter_usermode:
	mov rcx, rdi
	mov rsp, rsi
	mov r11, 0x202
	o64 sysret
