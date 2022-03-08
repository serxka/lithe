#pragma once

#include <utils/base/defs.h>

#include <kernel/arch/x86_64/context.h>

void syscall_init(void);
void syscall_gs_base(uintptr_t addr);
noreturn void sysret_enter_usermode(uint64_t rip, uint64_t rsp);
context_t syscall_get_new_context(void);
