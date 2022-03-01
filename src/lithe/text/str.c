#include <lithe/alloc/base.h>
#include <lithe/mem/mem.h>
#include <lithe/text/str.h>

str str_dup(str const s, allocator* alloc) {
	if (is_nullstr(s))
		return str$("");
	char* buf = (char*)alloc_malloc(alloc, s.len);
	memcpy(buf, s.buf, s.len);
	return str_n$(s.len, buf);
}

bool str_equ(str const s1, str const s2) {
	if (s1.len != s2.len)
		return false;
	for (size_t i = 0; i < s1.len; ++i)
		if (s1.buf[i] != s2.buf[i])
			return false;
	return true;
}
