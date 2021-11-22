#pragma once

#include <lithe/base/defs.h>

typedef struct {
	char _dummy;
} print_args;

#define FORMAT_ARGS(x) ((print_args){' '})
