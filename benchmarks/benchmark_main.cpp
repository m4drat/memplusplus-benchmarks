#include "benchmark/benchmark.h"
#include "allocator_api_override.h"

int main(int argc, char** argv)
{
    BenchmarkAllocatorInitialize();

    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();

    BenchmarkAllocatorFinalize();
}