#pragma once

#include <lithe/base/defs.h>
#include <lithe/io/print.h>

#define assert_not_null(expr)         \
	do {                          \
		if (expr == null)     \
			assert(expr); \
	} while (0)
