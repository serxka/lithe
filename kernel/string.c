#include <kernel/common.h>
#include <kernel/string.h>

void *memset(void *s, u8 c, size_t n) {
	u8 *p = (u8 *)s;
	for (size_t i = 0; i < n; ++i)
		p[i] = c;
	return s;
}
