#include "allocator_api_override.h"
#include "rpmalloc.h"
#include <cstdlib>

void BenchmarkAllocatorInitialize() {
    rpmalloc_initialize();
}

void BenchmarkAllocatorFinalize() {
    rpmalloc_finalize();
}


void* BenchmarkAllocate(std::size_t t_size) {
    return rpmalloc(t_size);
}

void BenchmarkDeallocate(void* t_ptr) {
    rpfree(t_ptr);
}
