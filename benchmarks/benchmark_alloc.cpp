#include "allocator_api_override.h"
#include "benchmark/benchmark.h"
#include "benchmark_constants.h"
#include "benchmark_utils.h"

/**
 * @brief Benchmarks the allocation speed for many different small allocation requests.
 */
static void BM_AllocateManySmallRandom(benchmark::State& state)
{
    for (auto _ : state) {
        // Somehow clear each allocator's state
        for (uint32_t iter = 0; iter < state.range(0); ++iter) {
            void* ptr =
                BenchmarkAllocate(smallSizes[bm::utils::XorshiftNext() % smallSizes.size()]);
            benchmark::DoNotOptimize(ptr);
        }
    }
}
BENCHMARK(BM_AllocateManySmallRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalAllocOpsRangeStart, g_totalAllocOpsRangeEndSmall);

/**
 * @brief Benchmarks the allocation speed for many different medium allocation requests.
 */
static void BM_AllocateManyMediumRandom(benchmark::State& state)
{
    for (auto _ : state) {
        // Somehow clear each allocator's state
        for (uint32_t iter = 0; iter < state.range(0); ++iter) {
            void* ptr =
                BenchmarkAllocate(mediumSizes[bm::utils::XorshiftNext() % mediumSizes.size()]);
            benchmark::DoNotOptimize(ptr);
        }
    }
}
BENCHMARK(BM_AllocateManyMediumRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalAllocOpsRangeStart, g_totalAllocOpsRangeEndMedium);

/**
 * @brief Benchmarks the allocation speed for many different huge allocations.
 */
static void BM_AllocateManyBigRandom(benchmark::State& state)
{
    for (auto _ : state) {
        for (uint32_t iter = 0; iter < state.range(0); ++iter) {
            void* ptr = BenchmarkAllocate(bigSizes[bm::utils::XorshiftNext() % bigSizes.size()]);
            benchmark::DoNotOptimize(ptr);
        }
    }
}
BENCHMARK(BM_AllocateManyBigRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalAllocOpsRangeStart, g_totalAllocOpsRangeEndBig);

/**
 * @brief Benchmarks the allocation speed for many differently sized objects (including small,
 * medium and big sizes).
 */
static void BM_AllocateManyRandom(benchmark::State& state)
{
    for (auto _ : state) {
        for (uint32_t iter = 0; iter < state.range(0); ++iter) {
            void* ptr =
                BenchmarkAllocate(combinedSizes[bm::utils::XorshiftNext() % combinedSizes.size()]);
            benchmark::DoNotOptimize(ptr);
        }
    }
}
BENCHMARK(BM_AllocateManyRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalAllocOpsRangeStart, g_totalAllocOpsRangeEndCombined);
