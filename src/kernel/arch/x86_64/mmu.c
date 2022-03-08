#include <kernel/vmm.h>
#include <utils/assert/assert.h>
#include <utils/assert/panic.h>
#include <utils/base/defs.h>
#include <utils/base/error.h>
#include <utils/base/result.h>
#include <utils/mem.h>
#include <utils/sync/spinlock.h>

#include <kernel/arch/x86_64/alloc.h>
#include <kernel/arch/x86_64/asm.h>
#include <kernel/arch/x86_64/mmu.h>

#define PMLN_IDX(ADDR, LVL)                                   \
	({                                                    \
		uint64_t _a = (uint64_t)(ADDR);               \
		uint64_t _s = (LVL)*9 + 12;                   \
		(_a & ((uint64_t)MEM_PAGE_MASK << _s)) >> _s; \
	})

extern symbol_t _kernel_vma;

// Held whenever any address space is being modified
static spinlock_t mmu_lock = {0};
// Pointer to the current address space
static addr_space_t current_space;
// Pointer to the kernel address space
static addr_space_t kernel_space;

static vm_addr_t vmm_phys2kernel(pm_addr_t addr) {
	return (pm_addr_t)_kernel_vma + addr;
}

void vmm_init(void) {
	spinlock_init$(mmu_lock);

	// Allocate and zero out a page for our kernel PML4
	kernel_space = unwrap$(alloc_first_frame());
	memset((void*)kernel_space, 0, MEM_PAGE_SIZE);

	// Identity map our kernel
	vm_range_t virt_range = range$(vm_range_t, 0x0, GiB(1));
	pm_range_t phys_range = range$(pm_range_t, 0x0, GiB(1));
	// don't actually map it for now
	unwrap$(vmm_map(kernel_space, virt_range, phys_range,
	                MEM_WRITABLE | MEM_KERNEL));

	// Map the upper -2GiB to -1GiB
	virt_range = (vm_range_t){(~(uint64_t)0) - GiB(2) + 1, GiB(1)};
	unwrap$(vmm_map(kernel_space, virt_range, phys_range,
	                MEM_WRITABLE | MEM_KERNEL));

	// Switch to new paging
	vmm_switch_space(kernel_space);
}

addr_space_t vmm_kernel_space(void) {
	return kernel_space;
}

addr_result_t vmm_virt2phys(addr_space_t space, vm_addr_t addr) {
	UNUSED(space), UNUSED(addr);
	unimplemented$();
	return OK(addr_result_t, 0);
}

space_result_t vmm_new_userspace(void) {
	addr_space_t space =
	        vmm_phys2kernel(try$(space_result_t, alloc_first_frame()));
	// Copy the base kernel PML4 into our address space
	memcpy((void*)space, (void*)kernel_space, MEM_PAGE_SIZE);

	return OK(space_result_t, space);
}

static addr_result_t vmm_get_pml(pml_table_t* table, size_t idx) {
	if (table->entries[idx].present)
		return OK(addr_result_t,
		          table->entries[idx].phys << MEM_PAGE_SHIFT);
	else
		return ERR(addr_result_t, ERR_BAD_ADDRESS);
}

static addr_result_t vmm_get_pml_update(pml_table_t* table, size_t idx,
                                        uint32_t mem_flags) {
	if (table->entries[idx].present) {
		table->entries[idx] = pml_new(
		        table->entries[idx].phys << MEM_PAGE_SHIFT, mem_flags);
		return OK(addr_result_t,
		          table->entries[idx].phys << MEM_PAGE_SHIFT);
	} else {
		return ERR(addr_result_t, ERR_BAD_ADDRESS);
	}
}

static addr_result_t vmm_get_pml_alloc(pml_table_t* table, size_t idx,
                                       uint32_t flags) {
	if (table->entries[idx].present) {
		return OK(addr_result_t,
		          table->entries[idx].phys << MEM_PAGE_SHIFT);
	} else {
		pm_addr_t addr = try$(addr_result_t, alloc_first_frame());
		memset((void*)addr, 0, MEM_PAGE_SIZE);
		table->entries[idx] = pml_new(addr, flags);
		return OK(addr_result_t, addr);
	}
}

static addr_result_t vmm_map_page(addr_space_t addr, vm_addr_t virt,
                                  pm_addr_t phys, uint32_t mem_flags) {
	// Navigate tables and allocated them if required
	pml_table_t* pml4 = (pml_table_t*)addr;
	pml_table_t* pml3 = (pml_table_t*)try$(
	        addr_result_t,
	        vmm_get_pml_alloc(pml4, PMLN_IDX(virt, 3), mem_flags));
	pml_table_t* pml2 = (pml_table_t*)try$(
	        addr_result_t,
	        vmm_get_pml_alloc(pml3, PMLN_IDX(virt, 2), mem_flags));
	pml_table_t* pml1 = (pml_table_t*)try$(
	        addr_result_t,
	        vmm_get_pml_alloc(pml2, PMLN_IDX(virt, 1), mem_flags));

	// Get the page or allocate it
	pml_t* page = pml1->entries + PMLN_IDX(virt, 0);
	if (page->present)
		panic$("page %x is already mapped to %x\r\n", virt, page->phys);

	*page = pml_new(phys, mem_flags);

	return OK(addr_result_t, virt);
}

maybe_t vmm_map(addr_space_t space, vm_range_t virt_range,
                pm_range_t phys_range, uint32_t mem_flags) {
	assert_true$(range_is_page_aligned(virt_range));
	assert_true$(range_is_page_aligned(phys_range));
	spinlock_take$(mmu_lock);

	if (virt_range.size != phys_range.size) {
		panic$("virt_range.size and phys_range.size must be the same. "
		       "'%x' != '%x'",
		       virt_range.size, phys_range.size);
		return FAILURE(ERR_BAD_PARAMETERS);
	}
	// later on split this up into bigger page chunks (2mb) rather then lots
	// of smaller pages
	for (size_t i = 0; i < (virt_range.size / MEM_PAGE_SIZE); ++i) {
		addr_result_t res =
		        vmm_map_page(space,
		                     align_down$(virt_range.base, MEM_PAGE_SIZE)
		                             + i * MEM_PAGE_SIZE,
		                     align_down$(phys_range.base, MEM_PAGE_SIZE)
		                             + i * MEM_PAGE_SIZE,
		                     mem_flags);
		if (res.fail)
			return FAILURE(res.err);
	}

	spinlock_give$(mmu_lock);
	return SUCCESS;
}

maybe_t vmm_alloc_range(addr_space_t space, vm_range_t virt_range,
                        uint32_t mem_flags) {
	assert_true$(range_is_page_aligned(virt_range));
	spinlock_take$(mmu_lock);

	for (size_t i = 0; i < (virt_range.size / MEM_PAGE_SIZE); ++i) {
		pm_addr_t phys = try$(maybe_t, alloc_first_frame());
		addr_result_t res =
		        vmm_map_page(space,
		                     align_down$(virt_range.base, MEM_PAGE_SIZE)
		                             + i * MEM_PAGE_SIZE,
		                     phys, mem_flags);
		if (res.fail)
			return FAILURE(res.err);
	}

	spinlock_give$(mmu_lock);
	return SUCCESS;
}

static addr_result_t vmm_unmap_page(addr_space_t addr, vm_addr_t virt) {
	// Navigate tables
	pml_table_t* pml4 = (pml_table_t*)addr;
	pml_table_t* pml3 = (pml_table_t*)try$(
	        addr_result_t, vmm_get_pml(pml4, PMLN_IDX(virt, 3)));
	pml_table_t* pml2 = (pml_table_t*)try$(
	        addr_result_t, vmm_get_pml(pml3, PMLN_IDX(virt, 2)));
	pml_table_t* pml1 = (pml_table_t*)try$(
	        addr_result_t, vmm_get_pml(pml2, PMLN_IDX(virt, 1)));

	// Get the page
	pml_t* page = pml1->entries + PMLN_IDX(virt, 0);
	assert_true$(page->present);
	*page = pml_empty();

	return OK(addr_result_t, virt);
}

void vmm_unmap(addr_space_t space, vm_range_t range) {
	assert_true$(range_is_page_aligned(range));
	spinlock_take$(mmu_lock);

	// Loop through each page and unmap it
	for (size_t i = 0; i < (range.size / MEM_PAGE_SIZE); ++i)
		vmm_unmap_page(space, align_down$(range.base, MEM_PAGE_SIZE)
		                              + i * MEM_PAGE_SIZE);

	spinlock_give$(mmu_lock);
}

static maybe_t vmm_set_flags_page(addr_space_t addr, vm_addr_t virt,
                                  uint32_t mem_flags) {
	// Navigate tables
	pml_table_t* pml4 = (pml_table_t*)addr;
	pml_table_t* pml3 = (pml_table_t*)try$(
	        maybe_t,
	        vmm_get_pml_update(pml4, PMLN_IDX(virt, 3), mem_flags));
	pml_table_t* pml2 = (pml_table_t*)try$(
	        maybe_t,
	        vmm_get_pml_update(pml3, PMLN_IDX(virt, 2), mem_flags));
	pml_table_t* pml1 = (pml_table_t*)try$(
	        maybe_t,
	        vmm_get_pml_update(pml2, PMLN_IDX(virt, 1), mem_flags));

	// Get the page
	pml_t* page = pml1->entries + PMLN_IDX(virt, 0);
	assert_true$(page->present);
	*page = pml_new(page->phys << MEM_PAGE_SHIFT, mem_flags);

	return SUCCESS;
}

maybe_t vmm_set_flags(addr_space_t space, vm_range_t range,
                      uint32_t mem_flags) {
	assert_true$(range_is_page_aligned(range));
	spinlock_take$(mmu_lock);

	// Loop through each page and unmap it
	for (size_t i = 0; i < (range.size / MEM_PAGE_SIZE); ++i)
		vmm_set_flags_page(space,
		                   align_down$(range.base, MEM_PAGE_SIZE)
		                           + i * MEM_PAGE_SIZE,
		                   mem_flags);

	spinlock_give$(mmu_lock);
	return SUCCESS;
}

void vmm_switch_space(addr_space_t space) {
	current_space = space;
	asm_write_cr3((uint64_t)current_space);
}

static void vmm_destroy_space_pml(uint8_t level, pml_table_t* table,
                                  size_t upto) {
	if (level < 1)
		return;

	for (size_t i = 0; i < upto; ++i) {
		addr_result_t pml = vmm_get_pml(table, i);
		if (pml.fail)
			continue;
		vmm_destroy_space_pml(level - 1, (pml_table_t*)pml.ok, 512);
	}
	alloc_frame_clear((uintptr_t)table);
}

void vmm_destroy_space(addr_space_t space) {
	// Destroy all pages in the address space up to the halfway point. This
	// is because kernel pages are inserted afterwards and we don't want to
	// deallocate those.
	vmm_destroy_space_pml(4, (pml_table_t*)space, 255);
}
