#include "allocator_api_override.h"
#include "benchmark/benchmark.h"
#include "benchmark_constants.h"
#include "benchmark_utils.h"

#define BENCH_DEALLOC_BLUEPRINT(BM_NAME, sizes)                                                    \
    static void BM_NAME(benchmark::State& state)                                                   \
    {                                                                                              \
        for (auto _ : state) {                                                                     \
            state.PauseTiming();                                                                   \
            bm::utils::XorshiftInit(1337 + 2);                                                     \
            std::vector<void*> pointers;                                                           \
            pointers.reserve(state.range(0));                                                      \
            for (uint32_t iter = 0; iter < state.range(0); ++iter) {                               \
                pointers.emplace_back(                                                             \
                    BenchmarkAllocate(sizes[bm::utils::XorshiftNext() % sizes.size()]));           \
            }                                                                                      \
            state.ResumeTiming();                                                                  \
            for (auto ptr : pointers)                                                              \
                BenchmarkDeallocate(ptr);                                                          \
        }                                                                                          \
    }

/**
 * @brief Benchmarks the deallocation speed for many different small sizes.
 */
BENCH_DEALLOC_BLUEPRINT(DISABLED_BM_DeallocateManySmallRandom, g_smallSizes)
BENCHMARK(DISABLED_BM_DeallocateManySmallRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalDeallocOpsRangeStart, g_totalDeallocOpsRangeEnd)
    ->Unit(benchmark::kMicrosecond);

/**
 * @brief Benchmarks the deallocation speed for many different medium sizes.
 */
BENCH_DEALLOC_BLUEPRINT(DISABLED_BM_DeallocateManyMediumRandom, g_mediumSizes)
BENCHMARK(DISABLED_BM_DeallocateManyMediumRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalDeallocOpsRangeStart, g_totalDeallocOpsRangeEnd)
    ->Unit(benchmark::kMicrosecond);

/**
 * @brief Benchmarks the deallocation speed for many different big sizes.
 */
BENCH_DEALLOC_BLUEPRINT(DISABLED_BM_DeallocateManyBigRandom, g_bigSizes)
BENCHMARK(DISABLED_BM_DeallocateManyBigRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalDeallocOpsRangeStart, g_totalDeallocOpsRangeEnd)
    ->Unit(benchmark::kMicrosecond);

/**
 * @brief Benchmarks the deallocation speed for many differently sized objects (including small,
 * medium and big sizes).
 */
BENCH_DEALLOC_BLUEPRINT(BM_DeallocateManyRandom, g_combinedSizes)
BENCHMARK(BM_DeallocateManyRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalDeallocOpsRangeStart, g_totalDeallocOpsRangeEnd)
    ->Unit(benchmark::kMicrosecond);
