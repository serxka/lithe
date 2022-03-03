#pragma once

#include <lithe/base/defs.h>
#include <lithe/base/attributes.h>

typedef struct {
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rbp, rdi, rsi, rdx, rcx,
	        rbx, rax, rsp, rip, rflags;
} PACKED regs_t;

typedef struct {
	uintptr_t kernel_stack;
	uintptr_t user_stack;
	regs_t regs;
} context_t;
