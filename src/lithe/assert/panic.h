#pragma once

#include <lithe/base/defs.h>
#include <lithe/base/stdinc.h>
#include <lithe/io/format.h>
#include <lithe/text/str.h>

typedef struct {
	uint32_t line;
	str filename;
	str function;
} src_location;

#define current_location                                      \
	(src_location) {                                      \
		__LINE__, str$(__FILENAME__), str$(__func__), \
	}

noreturn void _panic(src_location location, str fmt, print_args args);

#define panic$(fmt, ...) \
	_panic(current_location, str$(fmt), FORMAT_ARGS(__VA_ARGS__))
