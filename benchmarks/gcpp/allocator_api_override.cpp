#include "allocator_api_override.h"
#include "gcpp/gpage_allocator.h"
#include "gcpp/deferred_heap.h"

void BenchmarkAllocatorInitialize() { return; }
void BenchmarkAllocatorFinalize() { return; }

void* BenchmarkAllocate(std::size_t t_size) {
    return nullptr;
    // return heap.allocate<char>(t_size);
}

void BenchmarkDeallocate(void* t_ptr) {
    return;
    // gcpp::page.deallocate((gcpp::byte*)t_ptr);
}
