#pragma once

#include <utils/base/defs.h>

void arch_enable_interrupts(void);
void arch_disable_interrupts(void);
noreturn void arch_halt(void);
