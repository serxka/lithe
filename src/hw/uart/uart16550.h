#pragma once

#include <utils/base/defs.h>

#define RISCV_QEMU_UART_BASE (0x10000000)

typedef struct {
	uint8_t* base;
} uart16550_t;

uart16550_t uart16550_init(uintptr_t base);
void uart16550_put(uart16550_t* uart, uint8_t c);
uint8_t uart16550_get(uart16550_t* uart);
