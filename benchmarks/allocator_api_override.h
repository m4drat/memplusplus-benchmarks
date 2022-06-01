#include <cstddef>

#define FORCENOINLINE __attribute__((__noinline__))

extern FORCENOINLINE void BenchmarkAllocatorInitialize();
extern FORCENOINLINE void BenchmarkAllocatorFinalize();
extern FORCENOINLINE void* BenchmarkAllocate(std::size_t t_size);
extern FORCENOINLINE void BenchmarkDeallocate(void* t_ptr);

#undef FORCENOINLINE