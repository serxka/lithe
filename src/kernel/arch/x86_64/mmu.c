#include <kernel/vmm.h>
#include <lithe/base/defs.h>
#include <lithe/base/error.h>
#include <lithe/base/result.h>
#include <lithe/mem.h>
#include <lithe/sync/spinlock.h>

#include "alloc.h"
#include "mmu.h"

#define PMLN_IDX(ADDR, LVL)                                 \
	({                                                  \
		uint64_t _a = (uint64_t)(ADDR);             \
		uint64_t _s = (LVL)*9 + 12;                 \
		(_a & (uint64_t)MEM_PAGE_MASK << _s) >> _s; \
	})

extern symbol_t _kernel_vma;
extern symbol_t _end;

static spinlock mmu_lock = {0};
static addr_space current_space;
static addr_space kernel_space;

static pm_addr kern2phys_addr(vm_addr addr) {
	return (vm_addr)_kernel_vma - addr;
}

void vmm_init(void) {
	spinlock_init(mmu_lock);

	// Create our address space
	kernel_space = unwrap$(vmm_create_space());

	// Identity map our kernel
	vm_range virt_range = range$(vm_range, 0x0, GiB(2));
	pm_range phys_range = range$(pm_range, 0x0, GiB(2));
	unwrap$(vmm_map(kernel_space, virt_range, phys_range, 0));

	// Map the upper -2GiB
	virt_range = (vm_range){(~(size_t)0) - GiB(2), GiB(2)};
	unwrap$(vmm_map(kernel_space, virt_range, phys_range, 0));

	// Switch to new paging
	vmm_switch_space(kernel_space);
}

space_result vmm_create_space(void) {
	addr_space addr = (addr_space)try$(space_result, alloc_first_frame());
	memset(addr, 0, MEM_PAGE_SIZE);
	return OK(space_result, addr);
}

addr_space vmm_kernel_space(void) {
	return (addr_space)kern2phys_addr((vm_addr)kernel_space);
}

addr_result vmm_virt2phys(addr_space space, vm_addr addr) {
	UNUSED(space), UNUSED(addr);
	panic("function not implemented %s", "vmm_virt2phys");
	return OK(addr_result, 0);
}

void vmm_destroy_space(addr_space space) {
	UNUSED(space);
	panic("function not implemented %s", "vmm_destroy_space");
}

void vmm_switch_space(addr_space space) {
	current_space = space;
	pm_addr cr3 = kern2phys_addr((vm_addr)current_space);
	__asm__ __volatile__("mov %0, %%cr3" ::"r"(cr3));
}

static addr_result vmm_get_pml_alloc(pml_table *table, size_t idx,
				     uint32_t flags) {
	if (table->entries[idx].present) {
		return OK(addr_result,
			  table->entries[idx].phys << MEM_PAGE_SHIFT);
	} else {
		pm_addr addr = try$(addr_result, alloc_first_frame()) >> 12;
		memset((void *)addr, 0, MEM_PAGE_SIZE);
		table->entries[idx] = pml_new(addr, flags);
		return OK(addr_result, addr);
	}
}

static addr_result vmm_map_page(addr_space addr, vm_addr virt, pm_addr phys,
				uint32_t mem_flags) {
	// Navigate tables and allocated them if required
	pml_table *pml4 = (pml_table *)addr;
	pml_table *pml3 = (pml_table *)try$(
		addr_result,
		vmm_get_pml_alloc(pml4, PMLN_IDX(virt, 3),
				  mem_flags | MEM_WRITABLE | MEM_USER));
	pml_table *pml2 = (pml_table *)try$(
		addr_result,
		vmm_get_pml_alloc(pml3, PMLN_IDX(virt, 2),
				  mem_flags | MEM_WRITABLE | MEM_USER));
	pml_table *pml1 = (pml_table *)try$(
		addr_result,
		vmm_get_pml_alloc(pml2, PMLN_IDX(virt, 1),
				  mem_flags | MEM_WRITABLE | MEM_USER));
	pml *page = pml1->entries + PMLN_IDX(virt, 0);

	if (page->present)
		panic("page %x is already mapped to %x\r\n", virt, page->phys);

	*page = pml_new(phys, mem_flags);

	return OK(addr_result, virt);
}

maybe vmm_map(addr_space space, vm_range virt_range, pm_range phys_range,
	      uint32_t mem_flags) {
	// assert_true(range_is_page_aligned(virt_range));
	// assert_true(range_is_page_aligned(phys_range));
	spinlock_take(mmu_lock);

	if (virt_range.size != phys_range.size) {
		panic("virt_range.size and phys_range.size must be the same. '%x' != '%x'",
		      virt_range.size, phys_range.size);
		return FAILURE(ERR_BAD_PARAMETERS);
	}
	// later on split this up into bigger page chunks (2mb) rather then lots
	// of smaller pages
	for (size_t i = 0; i < (virt_range.size / MEM_PAGE_SIZE); ++i) {
		addr_result res = vmm_map_page(
			space,
			ALIGN_DOWN(virt_range.base, MEM_PAGE_SIZE) +
				i * MEM_PAGE_SIZE,
			ALIGN_DOWN(phys_range.base, MEM_PAGE_SIZE) +
				i * MEM_PAGE_SIZE,
			mem_flags);
		if (res.fail)
			return FAILURE(res.err);
	}

	spinlock_give(mmu_lock);
	return SUCCESS;
}

void vmm_unmap(addr_space space, vm_range range) {
	panic("function not implemented %s", "vmm_unmap");
	UNUSED(space), UNUSED(range);
}
