#pragma once

#include <utils/assert/base.h>
#include <utils/base/defs.h>
#include <utils/text/str.h>

noreturn void _panic(src_location_t location, const char* fmt, ...);

#define panic$(...) _panic(current_location$(), __VA_ARGS__)

#define unimplemented$() panic$("%s is not implemented\r\n", __func__)
