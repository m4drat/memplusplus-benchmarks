#include "allocator_api_override.h"
#include "benchmark/benchmark.h"
#include "benchmark_constants.h"
#include "benchmark_utils.h"

#define BENCH_ALLOC_DEALLOC_BLUEPRINT(BM_NAME, sizes)                                              \
    static void BM_NAME(benchmark::State& state)                                                   \
    {                                                                                              \
        bm::utils::XorshiftInit(1337 + 3);                                                         \
        for (auto _ : state) {                                                                     \
            for (uint32_t iter = 0; iter < state.range(0); ++iter) {                               \
                void* ptr = BenchmarkAllocate(sizes[bm::utils::XorshiftNext() % sizes.size()]);    \
                BenchmarkDeallocate(ptr);                                                          \
            }                                                                                      \
        }                                                                                          \
    }

/**
 * @brief Benchmarks combined allocation and deallocation speed for many different small sizes.
 */
BENCH_ALLOC_DEALLOC_BLUEPRINT(DISABLED_BM_AllocateDeallocateManySmallRandom, g_smallSizes);
BENCHMARK(DISABLED_BM_AllocateDeallocateManySmallRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalOpsRangeStart, g_totalOpsRangeEnd)
    ->Unit(benchmark::kMicrosecond);

/**
 * @brief Benchmarks combined allocation and deallocation speed for many different medium sizes.
 */
BENCH_ALLOC_DEALLOC_BLUEPRINT(DISABLED_BM_AllocateDeallocateManyMediumRandom, g_mediumSizes);
BENCHMARK(DISABLED_BM_AllocateDeallocateManyMediumRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalOpsRangeStart, g_totalOpsRangeEnd)
    ->Unit(benchmark::kMicrosecond);

/**
 * @brief Benchmarks combined allocation and deallocation speed for many different big sizes.
 */
BENCH_ALLOC_DEALLOC_BLUEPRINT(DISABLED_BM_AllocateDeallocateManyBigRandom, g_bigSizes);
BENCHMARK(DISABLED_BM_AllocateDeallocateManyBigRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalOpsRangeStart, g_totalOpsRangeEnd)
    ->Unit(benchmark::kMicrosecond);

/**
 * @brief Benchmarks combined allocation and deallocation speed for many differently sized
 objects
 * (including small, medium and big sizes).
 */
BENCH_ALLOC_DEALLOC_BLUEPRINT(BM_AllocateDeallocateManyRandom, g_combinedSizes);
BENCHMARK(BM_AllocateDeallocateManyRandom)
    ->RangeMultiplier(2)
    ->Range(g_totalOpsRangeStart, g_totalOpsRangeEnd)
    ->Unit(benchmark::kMicrosecond);
