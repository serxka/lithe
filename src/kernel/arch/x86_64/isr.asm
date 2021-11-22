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

%macro popaq 0
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
	pop rax
%endmacro

%macro isr_stub 2
[extern %2]
[global %1]
%1:
	pushaq
	mov rdi, rsp
	call %2
	popaq
	iretq
%endmacro

isr_stub page_fault_isr, page_fault_handler
