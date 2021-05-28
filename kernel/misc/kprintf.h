#ifndef _KERNEL_MISC_KPRINTF_H_
#define _KERNEL_MISC_KPRINTF_H_

#include <kernel/types.h>

#include <stdarg.h>

extern size_t (*kprintf_writter)(uint8_t*, size_t);

int vsnkprintf(char *buf, size_t len, const char *fmt, va_list ap);
int kprintf(const char *fmt, ...);

#endif // _KERNEL_MISC_KPRINTF_H_
