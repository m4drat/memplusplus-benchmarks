#include "allocator_api_override.h"
#include "malloc.h"
#include <cstdint>

void BenchmarkAllocatorInitialize()
{
    return;
}
void BenchmarkAllocatorFinalize()
{
    return;
}

void* BenchmarkAllocate(std::size_t t_size)
{
    return malloc(t_size);
}

void BenchmarkDeallocate(void* t_ptr)
{
    free(t_ptr);
}
