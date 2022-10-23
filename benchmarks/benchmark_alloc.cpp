#include "allocator_api_override.h"
#include "benchmark/benchmark.h"
#include "benchmark_constants.h"
#include "benchmark_utils.h"

#define BENCH_ALLOC_BLUEPRINT(sizes)                                                               \
    do {                                                                                           \
        state.PauseTiming();                                                                       \
        bm::utils::XorshiftInit(1337);                                                             \
        std::vector<void*> pointers;                                                               \
        pointers.reserve(state.range(0));                                                          \
        state.ResumeTiming();                                                                      \
        for (uint32_t iter = 0; iter < state.range(0); ++iter) {                                   \
            pointers.emplace_back(                                                                 \
                BenchmarkAllocate(sizes[bm::utils::XorshiftNext() % sizes.size()]));               \
        }                                                                                          \
        state.PauseTiming();                                                                       \
        for (auto ptr : pointers)                                                                  \
            BenchmarkDeallocate(ptr);                                                              \
        state.ResumeTiming();                                                                      \
    } while (0)

/**
 * @brief Benchmarks the allocation speed for many different small allocation requests.
 */
static void BM_AllocateManySmallRandom(benchmark::State& state)
{
    for (auto _ : state) {
        BENCH_ALLOC_BLUEPRINT(g_smallSizes);
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
        BENCH_ALLOC_BLUEPRINT(g_mediumSizes);
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
        BENCH_ALLOC_BLUEPRINT(g_bigSizes);
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
        BENCH_ALLOC_BLUEPRINT(g_combinedSizes);
    }
}
BENCHMARK(BM_AllocateManyRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalAllocOpsRangeStart, g_totalAllocOpsRangeEndCombined);
