// #include "allocator_api_override.h"
// #include "benchmark/benchmark.h"
// #include "benchmark_constants.h"

// /**
//  * @brief Benchmarks combined allocation and deallocation speed for many different small sizes.
//  */
// static void BM_AllocateDeallocateManySmallRandom(benchmark::State& state)
// {
//     for (auto _ : state) {
//         void* ptr = BenchmarkAllocate(1024);
//         benchmark::DoNotOptimize(ptr);

//         state.PauseTiming();
//         BenchmarkDeallocate(ptr);
//         state.ResumeTiming();

//         benchmark::ClobberMemory();
//     }
// }
// BENCHMARK(BM_AllocateDeallocateManySmallRandom)
//     ->RangeMultiplier(2)
//     ->Range(g_totalOpsRangeStart, g_totalOpsRangeEnd);

// /**
//  * @brief Benchmarks combined allocation and deallocation speed for many different medium sizes.
//  */
// static void BM_AllocateDeallocateManyMediumRandom(benchmark::State& state)
// {
//     for (auto _ : state) {
//         void* ptr = BenchmarkAllocate(1024);
//         benchmark::DoNotOptimize(ptr);

//         state.PauseTiming();
//         BenchmarkDeallocate(ptr);
//         state.ResumeTiming();

//         benchmark::ClobberMemory();
//     }
// }
// BENCHMARK(BM_AllocateDeallocateManyMediumRandom)
//     ->RangeMultiplier(2)
//     ->Range(g_totalOpsRangeStart, g_totalOpsRangeEnd);

// /**
//  * @brief Benchmarks combined allocation and deallocation speed for many different big sizes.
//  */
// static void BM_AllocateDeallocateManyBigRandom(benchmark::State& state)
// {
//     for (auto _ : state) {
//         void* ptr = BenchmarkAllocate(1024);
//         benchmark::DoNotOptimize(ptr);

//         state.PauseTiming();
//         BenchmarkDeallocate(ptr);
//         state.ResumeTiming();

//         benchmark::ClobberMemory();
//     }
// }
// BENCHMARK(BM_AllocateDeallocateManyBigRandom)
//     ->RangeMultiplier(2)
//     ->Range(g_totalOpsRangeStart, g_totalOpsRangeEnd);

// /**
//  * @brief Benchmarks combined allocation and deallocation speed for many differently sized
//  objects
//  * (including small, medium and big sizes).
//  */
// static void BM_AllocateDeallocateManyRandom(benchmark::State& state)
// {
//     for (auto _ : state) {
//         void* ptr = BenchmarkAllocate(1024);
//         benchmark::DoNotOptimize(ptr);

//         state.PauseTiming();
//         BenchmarkDeallocate(ptr);
//         state.ResumeTiming();

//         benchmark::ClobberMemory();
//     }
// }
// BENCHMARK(BM_AllocateDeallocateManyRandom)
//     ->RangeMultiplier(2)
//     ->Range(g_totalOpsRangeStart, g_totalOpsRangeEnd);
