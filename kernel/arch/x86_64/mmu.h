#ifndef _KERNEL_ARCH_X86_64_MMU_H_
#define _KERNEL_ARCH_X86_64_MMU_H_

#include <kernel/common.h>

typedef u64 phys_t;
typedef u64 logi_t;

typedef union pml_u {
	struct {
		u64 present : 1;
		u64 writable : 1;
		u64 user : 1;
		u64 writethrough : 1;
		u64 nocache : 1;
		u64 accessed : 1;
		u64 dirty : 1;
		u64 size : 1;
		u64 gloabl : 1;
		u64 _i : 3;
		u64 pat : 1;
		u64 phys : 50;
		u64 xd : 1;
	} bits;
	u64 raw;
} pml_t;
static_assert(sizeof(pml_t) == 8, "pml_t was not 64-bits");

void mmu_init(size_t mem_amount);
// Map a kernel logical address to a physical one
phys_t mmu_kernel2phys(logi_t addr);
void mmu_switch_tree(phys_t addr);
void map(void);

#endif // _KERNEL_ARCH_X86_64_MMU_H_
