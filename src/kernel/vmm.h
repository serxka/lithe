#pragma once

#include <utils/base/defs.h>
#include <utils/base/error.h>
#include <utils/base/range.h>

#define MEM_WRITABLE (0x1)
#define MEM_USER (0x2)
#define MEM_KERNEL (0x4)

typedef uintptr_t addr_space_t;

typedef uintptr_t pm_addr_t;
typedef uintptr_t vm_addr_t;
typedef range_t(pm_addr_t) pm_range_t;
typedef range_t(vm_addr_t) vm_range_t;

typedef result_t(uintptr_t, error_t) addr_result_t;
typedef result_t(addr_space_t, error_t) space_result_t;

void vmm_init(void);
space_result_t vmm_new_userspace(void);
addr_space_t vmm_kernel_space(void);
addr_result_t vmm_virt2phys(addr_space_t space, vm_addr_t addr);
void vmm_destroy_space(addr_space_t space);
void vmm_switch_space(addr_space_t space);
maybe_t vmm_map(addr_space_t space, vm_range_t virt_range,
                pm_range_t phys_range, uint32_t mem_flags);
maybe_t vmm_set_flags(addr_space_t space, vm_range_t range, uint32_t mem_flags);
maybe_t vmm_alloc_range(addr_space_t space, vm_range_t virt_range,
                        uint32_t mem_flags);
void vmm_unmap(addr_space_t space, vm_range_t range);
