#include "allocator_api_override.h"
#include "benchmark/benchmark.h"
#include "benchmark_constants.h"
#include "benchmark_utils.h"

#define BENCH_ALLOC_BLUEPRINT(BM_NAME, sizes)                                                      \
    static void BM_NAME(benchmark::State& state)                                                   \
    {                                                                                              \
        for (auto _ : state) {                                                                     \
            state.PauseTiming();                                                                   \
            bm::utils::XorshiftInit(1337 + 1);                                                     \
            std::vector<void*> pointers;                                                           \
            pointers.reserve(state.range(0));                                                      \
            state.ResumeTiming();                                                                  \
            for (uint32_t iter = 0; iter < state.range(0); ++iter) {                               \
                pointers.emplace_back(                                                             \
                    BenchmarkAllocate(sizes[bm::utils::XorshiftNext() % sizes.size()]));           \
            }                                                                                      \
            state.PauseTiming();                                                                   \
            for (auto ptr : pointers)                                                              \
                BenchmarkDeallocate(ptr);                                                          \
            state.ResumeTiming();                                                                  \
        }                                                                                          \
    }

/**
 * @brief Benchmarks the allocation speed for many different small allocation requests.
 */
BENCH_ALLOC_BLUEPRINT(DISABLED_BM_AllocateManySmallRandom, g_smallSizes);
BENCHMARK(DISABLED_BM_AllocateManySmallRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalAllocOpsRangeStart, g_totalAllocOpsRangeEndSmall)
    ->Unit(benchmark::kMicrosecond);

/**
 * @brief Benchmarks the allocation speed for many different medium allocation requests.
 */
BENCH_ALLOC_BLUEPRINT(DISABLED_BM_AllocateManyMediumRandom, g_mediumSizes);
BENCHMARK(DISABLED_BM_AllocateManyMediumRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalAllocOpsRangeStart, g_totalAllocOpsRangeEndMedium)
    ->Unit(benchmark::kMicrosecond);

/**
 * @brief Benchmarks the allocation speed for many different huge allocations.
 */
BENCH_ALLOC_BLUEPRINT(DISABLED_BM_AllocateManyBigRandom, g_bigSizes);
BENCHMARK(DISABLED_BM_AllocateManyBigRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalAllocOpsRangeStart, g_totalAllocOpsRangeEndBig)
    ->Unit(benchmark::kMicrosecond);

/**
 * @brief Benchmarks the allocation speed for many differently sized objects (including small,
 * medium and big sizes).
 */
BENCH_ALLOC_BLUEPRINT(BM_AllocateManyRandom, g_combinedSizes);
BENCHMARK(BM_AllocateManyRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalAllocOpsRangeStart, g_totalAllocOpsRangeEndCombined)
    ->Unit(benchmark::kMicrosecond);
