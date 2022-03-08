#pragma once

#include <utils/base/attributes.h>
#include <utils/base/defs.h>

#define _ASM_OP(op)                         \
	static INLINE void asm_##op(void) { \
		__asm__ __volatile__(#op);  \
	}
#define _ASM_MAKE_CRX(X)                                                \
	static INLINE uint64_t asm_read_cr##X(void) {                   \
		uint64_t v;                                             \
		__asm__ __volatile__("mov %%cr" #X ", %0" : "=r"(v) :); \
		return v;                                               \
	}                                                               \
	static INLINE void asm_write_cr##X(uint64_t v) {                \
		__asm__ __volatile__("mov %0, %%cr" #X ::"a"(v));       \
	}

_ASM_OP(cli)
_ASM_OP(sti)
_ASM_OP(hlt)
_ASM_MAKE_CRX(1)
_ASM_MAKE_CRX(2)
_ASM_MAKE_CRX(3)
_ASM_MAKE_CRX(4)

#undef _ASM_OP
#undef _ASM_MAKE_CRX

static INLINE uint8_t asm_inb(uint16_t port) {
	uint8_t data;
	__asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
	return data;
}

static INLINE void asm_outb(uint16_t port, uint8_t data) {
	__asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}

static INLINE noreturn void asm_halt(void) {
loop:
	asm_cli();
	asm_hlt();
	goto loop;
}

typedef enum {
	MSR_STAR = 0xc0000081,
	MSR_LSTAR = 0xc0000082,
	MSR_FMASK = 0xc0000084,
	MSR_GS_BASE = 0xc0000101,
	MSR_KERNEL_GS_BASE = 0xc0000102,
} asm_msr_register;

static INLINE uint64_t asm_read_msr(asm_msr_register msr) {
	uint32_t low, high;
	__asm__ __volatile__("rdmsr"
	                     : "=a"(low), "=d"(high)
	                     : "c"((uint64_t)msr));
	return (uint64_t)low | ((uint64_t)high << 32);
}

static INLINE void asm_write_msr(asm_msr_register msr, uint64_t v) {
	uint32_t low = v & 0xFFFFFFFF;
	uint32_t high = v >> 32;
	__asm__ __volatile__("wrmsr"
	                     :
	                     : "c"((uint64_t)msr), "a"(low), "d"(high));
}
