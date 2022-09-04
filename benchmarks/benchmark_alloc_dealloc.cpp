#include "allocator_api_override.h"
#include "benchmark/benchmark.h"

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

static void BM_AllocateSingle(benchmark::State& state)
{
    for (auto _ : state) {
        void* ptr = BenchmarkAllocate(1024);
        benchmark::DoNotOptimize(ptr);

        state.PauseTiming();
        BenchmarkDeallocate(ptr);
        state.ResumeTiming();

        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_AllocateSingle);

static void BM_DeallocateSingle(benchmark::State& state)
{
    for (auto _ : state) {
        state.PauseTiming();
        void* ptr = BenchmarkAllocate(1024);
        state.ResumeTiming();

        benchmark::DoNotOptimize(ptr);
        BenchmarkDeallocate(ptr);

        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_DeallocateSingle);
