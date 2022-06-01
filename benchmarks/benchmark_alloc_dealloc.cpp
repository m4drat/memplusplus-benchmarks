#include "benchmark/benchmark.h"
#include "allocator_api_override.h"

static void BM_AllocateDeallocate(benchmark::State& state)
{
    for (auto _ : state) {
        void* ptr = BenchmarkAllocate(1024);
        benchmark::DoNotOptimize(ptr);
        BenchmarkDeallocate(ptr);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_AllocateDeallocate);
