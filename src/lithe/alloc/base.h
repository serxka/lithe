#pragma once

#include <lithe/base/defs.h>

typedef void* (*alloc_aquire)(size_t size);
typedef void* (*alloc_resize)(void* ptr, size_t size);
typedef void (*alloc_release)(void* ptr);

typedef struct {
	alloc_aquire aquire;
	alloc_resize resize;
	alloc_release release;
} allocator;

void* alloc_malloc(allocator* alloc, size_t size);
void* alloc_realloc(allocator* alloc, void* ptr, size_t size);
void alloc_free(allocator* alloc, void* ptr);
