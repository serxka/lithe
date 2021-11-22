#include <lithe/base/defs.h>
#include <lithe/mem/mem.h>

void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
	return __builtin_memcpy(dest, src, n);
}

void *memmove(void *dest, const void *src, size_t n) {
	return __builtin_memmove(dest, src, n);
}

void *memset(void *s, int c, size_t n) {
	return __builtin_memset(s, c, n);
}
