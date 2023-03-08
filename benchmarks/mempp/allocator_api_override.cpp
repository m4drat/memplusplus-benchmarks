#include "allocator_api_override.h"
#include "mpplib/mpp.hpp"

void BenchmarkAllocatorInitialize()
{}

void BenchmarkAllocatorFinalize()
{
    return;
}

void* BenchmarkAllocate(std::size_t t_size)
{
    return mpp::Allocate(t_size);
}

void BenchmarkDeallocate(void* t_ptr)
{
    mpp::Deallocate(t_ptr);
}
