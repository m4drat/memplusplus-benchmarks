#include "allocator_api_override.h"
#include "mpplib/memory_manager.hpp"
#include "mpplib/mpp.hpp"
#include <memory>

void BenchmarkAllocatorInitialize()
{
    mpp::g_memoryManager = std::make_unique<mpp::MemoryManager>();
}

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
