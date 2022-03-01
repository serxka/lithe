#pragma once

#include <lithe/base/defs.h>
#include <lithe/base/error.h>
#include <lithe/base/range.h>

#define MEM_WRITABLE (0x1)
#define MEM_USER (0x2)
#define MEM_KERNEL (0x4)

typedef uintptr_t addr_space;

typedef uintptr_t pm_addr;
typedef uintptr_t vm_addr;
typedef range(pm_addr) pm_range;
typedef range(vm_addr) vm_range;

typedef result(uintptr_t, error) addr_result;
typedef result(addr_space, error) space_result;

void vmm_init(void);
space_result vmm_create_space(void);
addr_space vmm_kernel_space(void);
addr_result vmm_virt2phys(addr_space space, vm_addr addr);
void vmm_destroy_space(addr_space space);
void vmm_switch_space(addr_space space);
maybe vmm_map(addr_space space, vm_range virt_range, pm_range phys_range,
              uint32_t mem_flags);
void vmm_unmap(addr_space space, vm_range range);
