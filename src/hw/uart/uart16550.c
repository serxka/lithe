#include <hw/uart/uart16550.h>
#include <utils/base/defs.h>

#define INTERRUPT_ENABLE_REGISTER (0x1)
#define FIFO_CONTROL_REGISTER (0x2)
#define LINE_CONTROL_REGISTER (0x3)
#define LINE_STATUS_REGISTER (0x5)

uart16550_t uart16550_init(uintptr_t base) {
	uint8_t* ptr = (uint8_t*)base;

	// Set the word length to a byte
	*(ptr + LINE_CONTROL_REGISTER) = (1 << 0) | (1 << 0);
	// Enable FIFO
	*(ptr + FIFO_CONTROL_REGISTER) = (1 << 0);
	// Enable receiver interrupts
	*(ptr + INTERRUPT_ENABLE_REGISTER) = (1 << 0);

	return (uart16550_t){.base = (uint8_t*)base};
}

void uart16550_put(uart16550_t* uart, uint8_t c) {
	*(uart->base) = c;
}

uint8_t uart16550_get(uart16550_t* uart) {
	// Check if there a byte available, if so read and return it
	if (*(uart->base + LINE_STATUS_REGISTER) & 1) {
		return *(uart->base);
	} else {
		return 0;
	}
}
