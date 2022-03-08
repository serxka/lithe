#pragma once

#include <utils/assert/base.h>
#include <utils/base/defs.h>
#include <utils/text/str.h>

void _assert(src_location_t location, bool res, char* expr, str_t msg);

// Base assert macro
#define _assert$(expr, msg) \
	_assert(current_location$(), (expr), #expr, str$(msg))

// Overloadable assert macro
#define _assert_2(expr, msg) _assert$(expr, msg)
#define _assert_1(expr) _assert_2(expr, "")
#define assert$(...) MACRO_OVERLOAD(_assert_, __VA_ARGS__)(__VA_ARGS__)

// Overloadable assert true macro
#define _assert_true_2(expr, msg) _assert$(!(expr), msg)
#define _assert_true_1(expr) _assert_true_2(expr, "")
#define assert_true$(...) \
	MACRO_OVERLOAD(_assert_true_, __VA_ARGS__)(__VA_ARGS__)
