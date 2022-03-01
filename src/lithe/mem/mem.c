#include <lithe/base/defs.h>
#include <lithe/mem/mem.h>

void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
	uint8_t* d = (uint8_t*)dest;
	uint8_t* s = (uint8_t*)src;
	while (n--)
		*d++ = *s++;
	return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
	if (dest == src)
		return dest;

	uint8_t* d = (uint8_t*)dest;
	uint8_t* s = (uint8_t*)src;
	if (d < s) {
		while (n--)
			*d++ = *s++;
	} else {
		d += n;
		s += n;
		while (n--)
			*--d = *--s;
	}
	return dest;
}

void* memset(void* dest, int c, size_t n) {
	uint8_t* d = (uint8_t*)dest;
	while (n--)
		*d++ = c;
	return dest;
}
