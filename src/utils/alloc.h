#pragma once

#include <utils/base/defs.h>

typedef void* (*alloc_aquire)(size_t size);
typedef void* (*alloc_resize)(void* ptr, size_t size);
typedef void (*alloc_release)(void* ptr);

typedef struct {
	alloc_aquire aquire;
	alloc_resize resize;
	alloc_release release;
} allocator_t;

void* alloc_malloc(allocator_t* alloc, size_t size);
void* alloc_realloc(allocator_t* alloc, void* ptr, size_t size);
void alloc_free(allocator_t* alloc, void* ptr);
