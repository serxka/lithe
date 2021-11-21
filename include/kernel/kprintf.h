#ifndef _KERNEL_KPRINTF_H_
#define _KERNEL_KPRINTF_H_

#include <kernel/common.h>

#include <stdarg.h>

extern size_t (*kprintf_writter)(u8 *, size_t);

int vsnkprintf(char *buf, size_t len, const char *fmt, va_list ap);
int kprintf(const char *fmt, ...);

#define panic(fmt, ...)                                      \
	do {                                                 \
		kprintf("[PANIC] " fmt "\r\n", __VA_ARGS__); \
		__asm__ __volatile__("cli");                 \
		__asm__ __volatile__("hlt");                 \
	} while (0)

#endif // _KERNEL_KPRINTF_H_
