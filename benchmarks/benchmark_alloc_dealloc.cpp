#include "allocator_api_override.h"
#include "benchmark/benchmark.h"
#include "benchmark_constants.h"
#include "benchmark_utils.h"

/**
 * @brief Benchmarks combined allocation and deallocation speed for many different small sizes.
 */
static void BM_AllocateDeallocateManySmallRandom(benchmark::State& state)
{
    bm::utils::XorshiftInit(1337);
    for (auto _ : state) {
        for (uint32_t iter = 0; iter < state.range(0); ++iter) {
            void* ptr =
                BenchmarkAllocate(g_smallSizes[bm::utils::XorshiftNext() % g_smallSizes.size()]);
            BenchmarkDeallocate(ptr);
        }
    }
}
BENCHMARK(BM_AllocateDeallocateManySmallRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalOpsRangeStart, g_totalOpsRangeEnd);

/**
 * @brief Benchmarks combined allocation and deallocation speed for many different medium sizes.
 */
static void BM_AllocateDeallocateManyMediumRandom(benchmark::State& state)
{
    bm::utils::XorshiftInit(1337);
    for (auto _ : state) {
        for (uint32_t iter = 0; iter < state.range(0); ++iter) {
            void* ptr =
                BenchmarkAllocate(g_mediumSizes[bm::utils::XorshiftNext() % g_mediumSizes.size()]);
            BenchmarkDeallocate(ptr);
        }
    }
}
BENCHMARK(BM_AllocateDeallocateManyMediumRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalOpsRangeStart, g_totalOpsRangeEnd);

/**
 * @brief Benchmarks combined allocation and deallocation speed for many different big sizes.
 */
static void BM_AllocateDeallocateManyBigRandom(benchmark::State& state)
{
    bm::utils::XorshiftInit(1337);
    for (auto _ : state) {
        for (uint32_t iter = 0; iter < state.range(0); ++iter) {
            void* ptr =
                BenchmarkAllocate(g_bigSizes[bm::utils::XorshiftNext() % g_bigSizes.size()]);
            BenchmarkDeallocate(ptr);
        }
    }
}
BENCHMARK(BM_AllocateDeallocateManyBigRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalOpsRangeStart, g_totalOpsRangeEnd);

/**
 * @brief Benchmarks combined allocation and deallocation speed for many differently sized
 objects
 * (including small, medium and big sizes).
 */
static void BM_AllocateDeallocateManyRandom(benchmark::State& state)
{
    bm::utils::XorshiftInit(1337);
    for (auto _ : state) {
        for (uint32_t iter = 0; iter < state.range(0); ++iter) {
            void* ptr = BenchmarkAllocate(
                g_combinedSizes[bm::utils::XorshiftNext() % g_combinedSizes.size()]);
            BenchmarkDeallocate(ptr);
        }
    }
}
BENCHMARK(BM_AllocateDeallocateManyRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalOpsRangeStart, g_totalOpsRangeEnd);
