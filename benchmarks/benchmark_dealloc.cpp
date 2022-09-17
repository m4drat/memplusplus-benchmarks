#include "allocator_api_override.h"
#include "benchmark/benchmark.h"
#include "benchmark_constants.h"

/**
 * @brief Benchmarks the deallocation speed for many different small sizes.
 */
static void BM_DeallocateManySmallRandom(benchmark::State& state)
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
BENCHMARK(BM_DeallocateManySmallRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalDeallocOpsRangeStart, g_totalDeallocOpsRangeEnd);

/**
 * @brief Benchmarks the deallocation speed for many different medium sizes.
 */
static void BM_DeallocateManyMediumRandom(benchmark::State& state)
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
BENCHMARK(BM_DeallocateManyMediumRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalDeallocOpsRangeStart, g_totalDeallocOpsRangeEnd);

/**
 * @brief Benchmarks the deallocation speed for many different big sizes.
 */
static void BM_DeallocateManyBigRandom(benchmark::State& state)
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
BENCHMARK(BM_DeallocateManyBigRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalDeallocOpsRangeStart, g_totalDeallocOpsRangeEnd);

/**
 * @brief Benchmarks the deallocation speed for many differently sized objects (including small,
 * medium and big sizes).
 */
static void BM_DeallocateManyRandom(benchmark::State& state)
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
BENCHMARK(BM_DeallocateManyRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalDeallocOpsRangeStart, g_totalDeallocOpsRangeEnd);
