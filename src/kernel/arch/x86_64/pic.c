#include <kernel/kprintf.h>
#include <lithe/base/defs.h>

#include "asm.h"
#include "isr.h"

#define PIC1_CMD 0x20
#define PIC2_CMD 0xA0
#define PIC1_DATA 0x21
#define PIC2_DATA 0xA1

#define pic_eoi() asm_outb(PIC1_CMD, 0x20)

void pic_setmask(uint8_t irqline);
void pic_clearmask(uint8_t irqline);

void timer_handler(interrupt_frame_t* frame) {
	static int ticks;

	pic_eoi();
	++ticks;
	kprintf("timer called, tick: %d\r\n", ticks);

	UNUSED(frame);
}

static void pit_init(void) {
	// 1000hz
	uint16_t divisor = 1193182 / 1193;
	// set the config
	asm_outb(0x43, 0x36);
	// set the divisor
	asm_outb(0x40, divisor & 0xFF);
	asm_outb(0x40, divisor >> 8);
	// enable pit
	pic_clearmask(0);
}

void pic_init(void) {
	// remap the PIC
	asm_outb(PIC1_CMD, 0x11);
	asm_outb(PIC2_CMD, 0x11);
	asm_outb(PIC1_DATA, 0x20);
	asm_outb(PIC2_DATA, 0x28);
	asm_outb(PIC1_DATA, 0x04);
	asm_outb(PIC2_DATA, 0x02);
	asm_outb(PIC1_DATA, 0x01);
	asm_outb(PIC2_DATA, 0x01);
	asm_outb(PIC1_DATA, 0xFF);
	asm_outb(PIC2_DATA, 0xFF);

	// init the PIT
	// pit_init();
}

void pic_setmask(uint8_t irqline) {
	uint16_t port;
	uint8_t mask;

	if (irqline < 8)
		port = PIC1_DATA;
	else {
		port = PIC2_DATA;
		irqline -= 8;
	}

	mask = asm_inb(port) | (1 << irqline);
	asm_outb(port, mask);
}

void pic_clearmask(uint8_t irqline) {
	uint16_t port;
	uint8_t mask;

	if (irqline < 8)
		port = PIC1_DATA;
	else {
		port = PIC2_DATA;
		irqline -= 8;
	}

	mask = asm_inb(port) & ~(1 << irqline);
	asm_outb(port, mask);
}
