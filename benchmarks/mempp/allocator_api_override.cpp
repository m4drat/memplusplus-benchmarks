#include "allocator_api_override.h"
#include "mpplib/mpp.hpp"

void BenchmarkAllocatorInitialize() { return; }
void BenchmarkAllocatorFinalize() { return; }

void* BenchmarkAllocate(std::size_t t_size) {
    return mpp::MM::Allocate(t_size);
}

void BenchmarkDeallocate(void* t_ptr) {
    mpp::MM::Deallocate(t_ptr);
}
