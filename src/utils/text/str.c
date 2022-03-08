#include <utils/alloc.h>
#include <utils/mem/mem.h>
#include <utils/text/str.h>

str_t str_dup(str_t const s, allocator_t* alloc) {
	if (is_nullstr(s))
		return nullstr;
	char* buf = (char*)alloc_malloc(alloc, s.len);
	memcpy(buf, s.buf, s.len);
	return str_n$(s.len, buf);
}

bool str_equ(str_t const s1, str_t const s2) {
	if (s1.len != s2.len)
		return false;
	for (size_t i = 0; i < s1.len; ++i)
		if (s1.buf[i] != s2.buf[i])
			return false;
	return true;
}
