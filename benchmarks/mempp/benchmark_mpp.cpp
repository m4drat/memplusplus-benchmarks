#include "benchmark/benchmark.h"
#include "mpplib/memory_manager.hpp"


static void MppBenchmarkAllocate(benchmark::State& state) {
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(mpp::MemoryManager::Allocate(128));
    }
}
BENCHMARK(MppBenchmarkAllocate)->Unit(benchmark::kMillisecond);