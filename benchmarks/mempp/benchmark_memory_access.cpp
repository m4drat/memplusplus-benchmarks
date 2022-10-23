#include "allocator_api_override.h"
#include "benchmark/benchmark.h"
#include "benchmark_constants.h"

// static void BM_AccessMemory(benchmark::State& state)
// {
//     for (auto _ : state) {
//         void* ptr = BenchmarkAllocate(1024);
//         benchmark::DoNotOptimize(ptr);
//         BenchmarkDeallocate(ptr);
//         benchmark::ClobberMemory();
//     }
// }
// BENCHMARK(BM_AccessMemory)->RangeMultiplier(2)->Range(rangeStart, rangeEnd);

// 1. Randomized accesses inside X separate subgraphs
// 2. Randomized accesses inside randomly layouted linked list
// 3. P2329R0: Move, Copy, and Locality at Scale