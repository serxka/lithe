#pragma once

#include <utils/base/attributes.h>
#include <utils/base/defs.h>

#define _ASM_OP(op)                                     \
	static INLINE void asm_##op(void) {             \
		__asm__ __volatile__(#op ::: "memory"); \
	}
#define _ASM_MAKE_CSR(CSR)                                                     \
	static INLINE uint64_t asm_read_##CSR(void) {                          \
		uint64_t v;                                                    \
		__asm__ __volatile__("csrr %0, " #CSR : "=r"(v) : : "memory"); \
		return v;                                                      \
	}                                                                      \
	static INLINE void asm_write_##CSR(uint64_t v) {                       \
		__asm__ __volatile__("csrw " #CSR ", %0"                       \
		                     :                                         \
		                     : "r"(v)                                  \
		                     : "memory");                              \
	}                                                                      \
	static INLINE void asm_set_bits_##CSR(uint64_t bits) {                 \
		__asm__ __volatile__("csrs " #CSR ", %0"                       \
		                     :                                         \
		                     : "r"(bits)                               \
		                     : "memory");                              \
	}                                                                      \
	static INLINE void asm_clear_bits_##CSR(uint64_t bits) {               \
		__asm__ __volatile__("csrc " #CSR ", %0"                       \
		                     :                                         \
		                     : "r"(bits)                               \
		                     : "memory");                              \
	}

_ASM_OP(wfi)
_ASM_MAKE_CSR(satp)
_ASM_MAKE_CSR(scause)
_ASM_MAKE_CSR(sepc)
_ASM_MAKE_CSR(sie)
_ASM_MAKE_CSR(sip)
_ASM_MAKE_CSR(sscratch)
_ASM_MAKE_CSR(sstatus)
_ASM_MAKE_CSR(svtec)

#undef _ASM_OP
#undef _ASM_MAKE_CSR

enum {
	SATP_SV48 = (9UL << 60),
};

enum {
	SIE_S_SOFTWARE = (1 << 1),
	SIE_S_TIME = (1 << 5),
};

enum {
	SSTATUS_SIE = (1 << 1),
};
