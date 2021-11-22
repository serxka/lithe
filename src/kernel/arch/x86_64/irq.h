#ifndef _KERNEL_ARCH_X86_64_IRQ_H_
#define _KERNEL_ARCH_X86_64_IRQ_H_

#include <lithe/base/defs.h>

typedef struct interrupt_frame_s {
	// Pushed by stub
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
	uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;

	// Pushed by CPU
	uint64_t rip, cs, rflags, rsp, ss;
} PACKED interrupt_frame_t;

typedef struct interrupt_frame_err_s {
	// Pushed by stub
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
	uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;

	// Error code, pushed by CPU
	uint64_t err_code;

	// Pushed by CPU
	uint64_t rip, cs, rflags, rsp, ss;
} PACKED interrupt_frame_err_t;

#endif // _KERNEL_ARCH_X86_64_IRQ_H_
