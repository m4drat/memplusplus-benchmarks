#include "allocator_api_override.h"
#include "benchmark/benchmark.h"
#include "benchmark_constants.h"
#include "benchmark_utils.h"

#define BENCH_DEALLOC_BLUEPRINT(sizes)                                                             \
    do {                                                                                           \
        state.PauseTiming();                                                                       \
        bm::utils::XorshiftInit(1337);                                                             \
        std::vector<void*> pointers;                                                               \
        pointers.reserve(state.range(0));                                                          \
        for (uint32_t iter = 0; iter < state.range(0); ++iter) {                                   \
            pointers.emplace_back(                                                                 \
                BenchmarkAllocate(sizes[bm::utils::XorshiftNext() % sizes.size()]));               \
        }                                                                                          \
        state.ResumeTiming();                                                                      \
        for (auto ptr : pointers)                                                                  \
            BenchmarkDeallocate(ptr);                                                              \
    } while (0)

/**
 * @brief Benchmarks the deallocation speed for many different small sizes.
 */
static void BM_DeallocateManySmallRandom(benchmark::State& state)
{
    for (auto _ : state) {
        BENCH_DEALLOC_BLUEPRINT(g_smallSizes);
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
        BENCH_DEALLOC_BLUEPRINT(g_mediumSizes);
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
        BENCH_DEALLOC_BLUEPRINT(g_bigSizes);
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
        BENCH_DEALLOC_BLUEPRINT(g_combinedSizes);
    }
}
BENCHMARK(BM_DeallocateManyRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalDeallocOpsRangeStart, g_totalDeallocOpsRangeEnd);
