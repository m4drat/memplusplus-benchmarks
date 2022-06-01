#include "allocator_api_override.h"
#include "mimalloc.h"

void BenchmarkAllocatorInitialize() { return; }
void BenchmarkAllocatorFinalize() { return; }

void* BenchmarkAllocate(std::size_t t_size) {
    return mi_malloc(t_size);
}

void BenchmarkDeallocate(void* t_ptr) {
    mi_free(t_ptr);
}
