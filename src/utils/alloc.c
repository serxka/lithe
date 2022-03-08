#include <utils/alloc.h>
#include <utils/mem/mem.h>

void* alloc_malloc(allocator_t* alloc, size_t size) {
	void* ptr = alloc->aquire(size);
	return ptr;
}

void* alloc_realloc(allocator_t* alloc, void* ptr, size_t size) {
	void* new = alloc->resize(ptr, size);
	return new;
}

void alloc_free(allocator_t* alloc, void* ptr) {
	alloc->release(ptr);
}
