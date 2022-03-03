#include <kernel/kprintf.h>
#include <kernel/vmm.h>
#include <lithe/base/defs.h>
#include <lithe/base/error.h>
#include <lithe/base/result.h>
#include <lithe/mem.h>
#include <lithe/sync/spinlock.h>

#include "alloc.h"
#include "mmu.h"
#include "asm.h"

#define PMLN_IDX(ADDR, LVL)                                   \
	({                                                    \
		uint64_t _a = (uint64_t)(ADDR);               \
		uint64_t _s = (LVL)*9 + 12;                   \
		(_a & ((uint64_t)MEM_PAGE_MASK << _s)) >> _s; \
	})

extern symbol_t _kernel_vma;

// Held whenever any address space is being modified
static spinlock mmu_lock = {0};
// Pointer to the current address space
static addr_space current_space;
// Pointer to the kernel address space
static addr_space kernel_space;

static vm_addr vmm_phys2kernel(pm_addr addr) {
	return (pm_addr)_kernel_vma + addr;
}

void vmm_init(void) {
	spinlock_init(mmu_lock);

	// Allocate and zero out a page for our kernel PML4
	kernel_space = unwrap$(alloc_first_frame());
	memset((void*)kernel_space, 0, MEM_PAGE_SIZE);

	// Identity map our kernel
	vm_range virt_range = range$(vm_range, 0x0, GiB(1));
	pm_range phys_range = range$(pm_range, 0x0, GiB(1));
	// don't actually map it for now
	unwrap$(vmm_map(kernel_space, virt_range, phys_range,
	                MEM_WRITABLE | MEM_KERNEL));

	// Map the upper -2GiB to -1GiB
	virt_range = (vm_range){(~(uint64_t)0) - GiB(2) + 1, GiB(1)};
	unwrap$(vmm_map(kernel_space, virt_range, phys_range,
	                MEM_WRITABLE | MEM_KERNEL));

	// Switch to new paging
	vmm_switch_space(kernel_space);
}

addr_space vmm_kernel_space(void) {
	return kernel_space;
}

addr_result vmm_virt2phys(addr_space space, vm_addr addr) {
	UNUSED(space), UNUSED(addr);
	unimplemented();
	return OK(addr_result, 0);
}

space_result vmm_new_userspace(void) {
	addr_space space =
	        vmm_phys2kernel(try$(space_result, alloc_first_frame()));
	// Copy the base kernel PML4 into our address space
	memcpy((void*)space, (void*)kernel_space, MEM_PAGE_SIZE);

	return OK(space_result, space);
}

static addr_result vmm_get_pml(pml_table* table, size_t idx) {
	if (table->entries[idx].present)
		return OK(addr_result,
		          table->entries[idx].phys << MEM_PAGE_SHIFT);
	else
		return ERR(addr_result, ERR_BAD_ADDRESS);
}

static addr_result vmm_get_pml_update(pml_table* table, size_t idx,
                                      uint32_t mem_flags) {
	if (table->entries[idx].present) {
		table->entries[idx] = pml_new(
		        table->entries[idx].phys << MEM_PAGE_SHIFT, mem_flags);
		return OK(addr_result,
		          table->entries[idx].phys << MEM_PAGE_SHIFT);
	} else {
		return ERR(addr_result, ERR_BAD_ADDRESS);
	}
}

static addr_result vmm_get_pml_alloc(pml_table* table, size_t idx,
                                     uint32_t flags) {
	if (table->entries[idx].present) {
		return OK(addr_result,
		          table->entries[idx].phys << MEM_PAGE_SHIFT);
	} else {
		pm_addr addr = try$(addr_result, alloc_first_frame());
		memset((void*)addr, 0, MEM_PAGE_SIZE);
		table->entries[idx] = pml_new(addr, flags);
		return OK(addr_result, addr);
	}
}

static addr_result vmm_map_page(addr_space addr, vm_addr virt, pm_addr phys,
                                uint32_t mem_flags) {
	// Navigate tables and allocated them if required
	pml_table* pml4 = (pml_table*)addr;
	pml_table* pml3 = (pml_table*)try$(
	        addr_result,
	        vmm_get_pml_alloc(pml4, PMLN_IDX(virt, 3), mem_flags));
	pml_table* pml2 = (pml_table*)try$(
	        addr_result,
	        vmm_get_pml_alloc(pml3, PMLN_IDX(virt, 2), mem_flags));
	pml_table* pml1 = (pml_table*)try$(
	        addr_result,
	        vmm_get_pml_alloc(pml2, PMLN_IDX(virt, 1), mem_flags));

	// Get the page or allocate it
	pml* page = pml1->entries + PMLN_IDX(virt, 0);
	if (page->present)
		panic("page %x is already mapped to %x\r\n", virt, page->phys);

	*page = pml_new(phys, mem_flags);

	return OK(addr_result, virt);
}

maybe vmm_map(addr_space space, vm_range virt_range, pm_range phys_range,
              uint32_t mem_flags) {
	assert(!range_is_page_aligned(virt_range));
	assert(!range_is_page_aligned(phys_range));
	spinlock_take(mmu_lock);

	if (virt_range.size != phys_range.size) {
		panic("virt_range.size and phys_range.size must be the same. "
		      "'%x' != '%x'",
		      virt_range.size, phys_range.size);
		return FAILURE(ERR_BAD_PARAMETERS);
	}
	// later on split this up into bigger page chunks (2mb) rather then lots
	// of smaller pages
	for (size_t i = 0; i < (virt_range.size / MEM_PAGE_SIZE); ++i) {
		addr_result res =
		        vmm_map_page(space,
		                     ALIGN_DOWN(virt_range.base, MEM_PAGE_SIZE)
		                             + i * MEM_PAGE_SIZE,
		                     ALIGN_DOWN(phys_range.base, MEM_PAGE_SIZE)
		                             + i * MEM_PAGE_SIZE,
		                     mem_flags);
		if (res.fail)
			return FAILURE(res.err);
	}

	spinlock_give(mmu_lock);
	return SUCCESS;
}

maybe vmm_alloc_range(addr_space space, vm_range virt_range,
                      uint32_t mem_flags) {
	assert(!range_is_page_aligned(virt_range));
	spinlock_take(mmu_lock);

	for (size_t i = 0; i < (virt_range.size / MEM_PAGE_SIZE); ++i) {
		pm_addr phys = try$(maybe, alloc_first_frame());
		addr_result res =
		        vmm_map_page(space,
		                     ALIGN_DOWN(virt_range.base, MEM_PAGE_SIZE)
		                             + i * MEM_PAGE_SIZE,
		                     phys, mem_flags);
		if (res.fail)
			return FAILURE(res.err);
	}

	spinlock_give(mmu_lock);
	return SUCCESS;
}

static addr_result vmm_unmap_page(addr_space addr, vm_addr virt) {
	// Navigate tables
	pml_table* pml4 = (pml_table*)addr;
	pml_table* pml3 = (pml_table*)try$(
	        addr_result, vmm_get_pml(pml4, PMLN_IDX(virt, 3)));
	pml_table* pml2 = (pml_table*)try$(
	        addr_result, vmm_get_pml(pml3, PMLN_IDX(virt, 2)));
	pml_table* pml1 = (pml_table*)try$(
	        addr_result, vmm_get_pml(pml2, PMLN_IDX(virt, 1)));

	// Get the page
	pml* page = pml1->entries + PMLN_IDX(virt, 0);
	assert(!page->present);
	*page = pml_empty();

	return OK(addr_result, virt);
}

void vmm_unmap(addr_space space, vm_range range) {
	assert(!range_is_page_aligned(range));
	spinlock_take(mmu_lock);

	// Loop through each page and unmap it
	for (size_t i = 0; i < (range.size / MEM_PAGE_SIZE); ++i)
		vmm_unmap_page(space, ALIGN_DOWN(range.base, MEM_PAGE_SIZE)
		                              + i * MEM_PAGE_SIZE);

	spinlock_give(mmu_lock);
}

static maybe vmm_set_flags_page(addr_space addr, vm_addr virt,
                                uint32_t mem_flags) {
	// Navigate tables
	pml_table* pml4 = (pml_table*)addr;
	pml_table* pml3 = (pml_table*)try$(
	        maybe, vmm_get_pml_update(pml4, PMLN_IDX(virt, 3), mem_flags));
	pml_table* pml2 = (pml_table*)try$(
	        maybe, vmm_get_pml_update(pml3, PMLN_IDX(virt, 2), mem_flags));
	pml_table* pml1 = (pml_table*)try$(
	        maybe, vmm_get_pml_update(pml2, PMLN_IDX(virt, 1), mem_flags));


	// Get the page
	pml* page = pml1->entries + PMLN_IDX(virt, 0);
	assert(!page->present);
	*page = pml_new(page->phys << MEM_PAGE_SHIFT, mem_flags);

	return SUCCESS;
}

maybe vmm_set_flags(addr_space space, vm_range range, uint32_t mem_flags) {
	assert(!range_is_page_aligned(range));
	spinlock_take(mmu_lock);

	// Loop through each page and unmap it
	for (size_t i = 0; i < (range.size / MEM_PAGE_SIZE); ++i)
		vmm_set_flags_page(space,
		                   ALIGN_DOWN(range.base, MEM_PAGE_SIZE)
		                           + i * MEM_PAGE_SIZE,
		                   mem_flags);

	spinlock_give(mmu_lock);
	return SUCCESS;
}

void vmm_switch_space(addr_space space) {
	current_space = space;
	asm_write_cr3((uint64_t)current_space);
}

static void vmm_destroy_space_pml(uint8_t level, pml_table* table,
                                  size_t upto) {
	if (level < 1)
		return;

	for (size_t i = 0; i < upto; ++i) {
		addr_result pml = vmm_get_pml(table, i);
		if (pml.fail)
			continue;
		vmm_destroy_space_pml(level - 1, (pml_table*)pml.ok, 512);
	}
	alloc_frame_clear((uintptr_t)table);
}

void vmm_destroy_space(addr_space space) {
	// Destroy all pages in the address space up to the halfway point. This
	// is because kernel pages are inserted afterwards and we don't want to
	// deallocate those.
	vmm_destroy_space_pml(4, (pml_table*)space, 255);
}
