#include <lithe/alloc/base.h>
#include <lithe/mem/mem.h>

void* alloc_malloc(allocator* alloc, size_t size) {
	void* ptr = alloc->aquire(size);
	return ptr;
}

void* alloc_realloc(allocator* alloc, void* ptr, size_t size) {
	void* new = alloc->resize(ptr, size);
	return new;
}

void alloc_free(allocator* alloc, void* ptr) {
	alloc->release(ptr);
}
