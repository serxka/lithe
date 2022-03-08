#pragma once

#include <utils/base/defs.h>
#include <utils/text/str.h>

typedef struct {
	uint32_t line;
	str_t file;
	str_t func;
} src_location_t;

#define current_location$()                               \
	(src_location_t) {                                \
		__LINE__, str$(__FILE__), str$(__func__), \
	}
