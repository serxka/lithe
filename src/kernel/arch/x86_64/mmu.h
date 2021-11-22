#ifndef _KERNEL_ARCH_X86_64_MMU_H_
#define _KERNEL_ARCH_X86_64_MMU_H_

#include <lithe/base/defs.h>

typedef uint64_t phys_t;
typedef uint64_t logi_t;

typedef union pml_u {
	struct {
		uint64_t present : 1;
		uint64_t writable : 1;
		uint64_t user : 1;
		uint64_t writethrough : 1;
		uint64_t nocache : 1;
		uint64_t accessed : 1;
		uint64_t dirty : 1;
		uint64_t size : 1;
		uint64_t gloabl : 1;
		uint64_t _i : 3;
		uint64_t pat : 1;
		uint64_t phys : 50;
		uint64_t xd : 1;
	} bits;
	uint64_t raw;
} pml_t;
static_assert(sizeof(pml_t) == 8, "pml_t was not 64-bits");

void mmu_init(size_t mem_amount);
// Map a kernel logical address to a physical one
phys_t mmu_kernel2phys(logi_t addr);
void mmu_switch_tree(phys_t addr);
void map(void);

#endif // _KERNEL_ARCH_X86_64_MMU_H_
