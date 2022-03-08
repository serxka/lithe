#pragma once

#include <utils/base/defs.h>

extern size_t (*kprintf_writter)(uint8_t*, size_t);

int vsnkprintf(char* buf, size_t len, const char* fmt, va_list ap);
int kprintf(const char* fmt, ...);
