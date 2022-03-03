#pragma once

#include <lithe/base/defs.h>

extern size_t (*kprintf_writter)(uint8_t*, size_t);

int vsnkprintf(char* buf, size_t len, const char* fmt, va_list ap);
int kprintf(const char* fmt, ...);

#define panic(fmt, ...)                                      \
	do {                                                 \
		kprintf("[PANIC] " fmt "\r\n", __VA_ARGS__); \
		__asm__ __volatile__("cli");                 \
		__asm__ __volatile__("hlt");                 \
	} while (0)

#define unimplemented() panic("%s is not implemented\r\n", __func__)

#define assert(expr)                                                   \
	do {                                                           \
		if (expr) {                                            \
			kprintf("Assertion Failed: (" #expr            \
			        ") function %s, file %s, line %d\r\n", \
			        __func__, __FILE__, __LINE__);         \
			__asm__ __volatile__("cli");                   \
			__asm__ __volatile__("hlt");                   \
		}                                                      \
	} while (0)

static inline noreturn void unreachable(void) {
	for (;;)
		;
}
