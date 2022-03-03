#pragma once

#include <lithe/base/attributes.h>
#include <lithe/base/defs.h>

typedef struct {
	// Pushed by stub
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
	uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;

	// Pushed by CPU
	uint64_t rip, cs, rflags, rsp, ss;
} PACKED interrupt_frame_t;

typedef struct {
	// Pushed by stub
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
	uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;

	// Error code, pushed by CPU
	uint64_t err_code;

	// Pushed by CPU
	uint64_t rip, cs, rflags, rsp, ss;
} PACKED interrupt_frame_err_t;

extern void double_fault_isr(void);
extern void gp_fault_isr(void);
extern void page_fault_isr(void);
extern void timer_irq(void);
