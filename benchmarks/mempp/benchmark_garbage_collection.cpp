#include "allocator_api_override.h"
#include "benchmark/benchmark.h"
#include "benchmark_constants.h"

// static void BM_GarbageCollect(benchmark::State& state)
// {
//     for (auto _ : state) {
//         void* ptr = BenchmarkAllocate(1024);
//         benchmark::DoNotOptimize(ptr);
//         BenchmarkDeallocate(ptr);
//         benchmark::ClobberMemory();
//     }
// }
// BENCHMARK(BM_GarbageCollect)->RangeMultiplier(2)->Range(rangeStart, rangeEnd);