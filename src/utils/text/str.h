#pragma once

#include <utils/alloc.h>
#include <utils/base/defs.h>

// Gets the length of a cstr
static inline size_t cstr_len(const char* s) {
	size_t l = 0;
	while (s[l++])
		;
	return l;
}

// A string which does not own its data
typedef struct {
	size_t len;
	const char* buf;
} str_t;

// String using a flexible array member
typedef struct {
	size_t len;
	char buf[];
} string_t;

static inline str_t str_forward(str_t s) {
	return s;
}
static inline str_t str_from_string(string_t* s) {
	return (str_t){s->len, s->buf};
}
static inline str_t str_from_cstr(const char* cstr) {
	return (str_t){cstr_len(cstr), cstr};
}
static inline str_t str_from_cstr8(const uint8_t* cstr) {
	return (str_t){cstr_len((const char*)cstr), (const char*)cstr};
}

// Creates a new `str_t` from possible string types
#define str$(literal) \
	_Generic((literal), \
		str_t: str_forward, \
		string_t*: str_from_string, \
		char* : str_from_cstr, \
		const char* : str_from_cstr, \
		uint8_t*: str_from_cstr8, \
		const uint8_t*: str_from_cstr8 \
	)(literal)

// Creates a new string with known length
#define str_n$(n, s) \
	(str_t) {    \
		n, s \
	}

#define nullstr (str$(""))
#define is_nullstr(s) (s.len == 0)
str_t str_dup(str_t const s, allocator_t* alloc);
bool str_equ(str_t const s1, str_t const s2);
