#include "allocator_api_override.h"
#include <cstdlib>

void BenchmarkAllocatorInitialize() { return; }
void BenchmarkAllocatorFinalize() { return; }

void* BenchmarkAllocate(std::size_t t_size) {
    return std::malloc(t_size);
}

void BenchmarkDeallocate(void* t_ptr) {
    std::free(t_ptr);
}
