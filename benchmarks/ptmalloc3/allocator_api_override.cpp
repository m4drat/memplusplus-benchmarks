#include "allocator_api_override.h"
#include <cstdlib>
#include "malloc-2.8.3.h"

void BenchmarkAllocatorInitialize() { return; }
void BenchmarkAllocatorFinalize() { return; }

void* BenchmarkAllocate(std::size_t t_size) {
    return dlmalloc(t_size);
}

void BenchmarkDeallocate(void* t_ptr) {
    dlfree(t_ptr);
}
