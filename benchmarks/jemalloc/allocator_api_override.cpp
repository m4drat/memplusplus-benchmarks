#include "allocator_api_override.h"
#include <jemalloc/jemalloc.h>

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
