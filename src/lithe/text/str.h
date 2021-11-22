#pragma once

#include <lithe/alloc/base.h>
#include <lithe/base/defs.h>

// Gets the length of a cstr
static inline size_t cstr_len(const char *s) {
	size_t l = 0;
	while (s[l++])
		;
	return l;
}

// A string which does not own its data
typedef struct {
	size_t len;
	const char *buf;
} str;

// String using a flexible array member
typedef struct {
	size_t len;
	char buf[];
} string;

static inline str str_forward(str s) {
	return s;
}
static inline str str_from_string(string *s) {
	return (str){s->len, s->buf};
}
static inline str str_from_cstr(const char *cstr) {
	return (str){cstr_len(cstr), cstr};
}
static inline str str_from_cstr8(const uint8_t *cstr) {
	return (str){cstr_len((const char *)cstr), (const char *)cstr};
}

// Creates a new `str` from possible string types
#define str$(literal) \
	_Generic((literal), \
		str: str_forward, \
		string*: str_from_string, \
		char* : str_from_cstr, \
		const char* : str_from_cstr, \
		uint8_t*: str_from_cstr8, \
		const uint8_t*: str_from_cstr8 \
	)(literal)

// Creates a new string with known length
#define str_n$(n, s) \
	(str) {      \
		n, s \
	}

#define nullstr (str$(""))
#define is_nullstr(s) (s.len == 0)
str str_dup(str const s, allocator *alloc);
bool str_equ(str const s1, str const s2);
