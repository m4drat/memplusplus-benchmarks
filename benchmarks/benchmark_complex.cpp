#include "allocator_api_override.h"
#include "benchmark/benchmark.h"
#include "benchmark_constants.h"
#include "benchmark_utils.h"

#include <algorithm>
#include <cstdint>
#include <iostream>

class Worker
{
public:
    enum class Sizes
    {
        SMALL = 0,
        MEDIUM,
        BIG,
        COUNT,
        COMBINED,
        INVALID
    };

    enum class Operation
    {
        ALLOC_SINGLE = 0,
        ALLOC_MANY,
        DEALLOC_SINGLE,
        DEALLOC_MANY,
        COUNT,
        INVALID
    };

    enum class AllocManyOpArg
    {
        ALLOC_MANY_RANDOM_SIZES = 0,
        ALLOC_MANY_CONSTANT_SIZES,
        COUNT
    };

    Worker(uint32_t t_totalOps = 1024 * 128,
           uint32_t t_totalAllocOps = std::numeric_limits<uint32_t>::max(),
           uint32_t t_totalFreeOps = std::numeric_limits<uint32_t>::max(),
           std::array<std::array<float, 4>, 4> t_transitionMatrix = m_defaultTransitionMatrix,
           int64_t t_maxMemoryConsumption = (int64_t)1024 * 1024 * 1024 * 1 /* 1024 Mb */,
           uint64_t t_xorshiftSeed = 0x133796A5FF21B3C1)
        : m_totalOps(t_totalOps)
        , m_maxMemoryConsumption(t_maxMemoryConsumption)
        , m_transitionMatrix(t_transitionMatrix)
    {
        if (t_totalAllocOps == std::numeric_limits<uint32_t>::max())
            m_totalAllocOps = t_totalOps * 3 / 5;
        else
            m_totalAllocOps = t_totalAllocOps;

        if (t_totalFreeOps == std::numeric_limits<uint32_t>::max())
            m_totalFreeOps = t_totalOps * 2 / 5;
        else
            m_totalFreeOps = t_totalFreeOps;

        m_activePtrs = std::move(std::vector<void*>(t_totalOps, nullptr));

        m_transitionsState = bm::utils::XorshiftNext(t_xorshiftSeed);
        m_sizesState = bm::utils::XorshiftNext(t_xorshiftSeed);

        CheckTransitionMatrix();
        CalculateScatters();
    }

    std::tuple<uint32_t, uint32_t, uint32_t> RunBenchmark()
    {
        while (m_ops++ <= m_totalOps) {
            auto op = GetNextOperation();
            switch (op) {
                case Operation::ALLOC_SINGLE:
                    // std::cout << m_ops << ". Alloc single" << std::endl;
                    AllocSingle(GetRandomSize());
                    break;
                case Operation::ALLOC_MANY:
                    // std::cout << m_ops << ". Alloc many" << std::endl;
                    AllocMany(NextMultipleAllocationsCount(),
                              (bm::utils::XorshiftNext(m_sizesState, 0.0f, 1.0f) > 0.8) ? true
                                                                                        : false);
                    break;
                case Operation::DEALLOC_SINGLE:
                    // std::cout << m_ops << ". Dealloc single" << std::endl;
                    FreeSingle();
                    break;
                case Operation::DEALLOC_MANY:
                    // std::cout << m_ops << ". Dealloc many" << std::endl;
                    FreeMany(NextMultipleDellocationsCount());
                    break;
                default:
                    break;
            }

            // std::cout << m_ops++ << ". "
            //           << "m_allocOps: " << m_allocOps << ". m_freeOps: " << m_freeOps <<
            //           std::endl;
            // std::cout << m_totalAllocated << std::endl;

            // if ((m_allocOps >= m_totalAllocOps && m_freeOps >= m_totalFreeOps))
            //     break;

            // m_ops++;
        }

        return { m_ops, m_allocOps, m_freeOps };
    }

    void CleanUp()
    {
        for (auto& ptr : m_activePtrs) {
            if (ptr) {
                BenchmarkDeallocate(ptr);
                ptr = nullptr;
            }
        }
    }

private:
    std::vector<void*> m_activePtrs;
    uint32_t m_totalAllocOps;
    uint32_t m_totalFreeOps;
    uint32_t m_totalOps;

    uint32_t m_allocOps = 0;
    uint32_t m_freeOps = 0;
    uint32_t m_ops = 0;

    int64_t m_maxMemoryConsumption;
    int64_t m_totalAllocated = 0;

    uint32_t m_allocScatter;
    uint32_t m_freeScatter;

    uint64_t m_transitionsState;
    uint64_t m_sizesState;

    uint32_t m_freeIdx = 0;
    uint32_t m_allocIdx = 0;
    uint64_t m_allocOpsIdx = 0;
    uint64_t m_freeOpsIdx = 0;

    Operation m_currentOp = Operation::ALLOC_SINGLE;
    std::array<std::array<float, 4>, 4> m_transitionMatrix;
    static constexpr std::array<std::array<float, 4>, 4> m_defaultTransitionMatrix{
        std::array<float, 4>{ 0.15, 0.0, 0.0, 0.85 },   // AllocateSingle
        std::array<float, 4>{ 0.65, 0.0, 0.35, 0.0 },   // DeallocateSingle
        std::array<float, 4>{ 0.0, 0.82, 0.0, 0.18 },   // AllocateMultiple
        std::array<float, 4>{ 0.07, 0.00, 0.93, 0.00 }, // DeallocateMultiple
    };

    void CheckTransitionMatrix()
    {
        for (auto& row : m_transitionMatrix) {
            float sum = 0.0f;
            for (auto& col : row) {
                sum += col;
            }

            if (sum != 1.0f) {
                throw std::runtime_error("Transition matrix is not valid");
            }
        }
    }

    Operation GetNextOperation()
    {
        auto& row = m_transitionMatrix[static_cast<uint32_t>(m_currentOp)];
        auto rand = bm::utils::XorshiftNext(m_transitionsState, 0.0f, 1.0f);
        float sum = 0.0f;
        for (uint32_t i = 0; i < row.size(); ++i) {
            sum += row[i];
            if (rand < sum) {
                m_currentOp = static_cast<Operation>(i);
                return m_currentOp;
            }
        }

        return Operation::INVALID;
    }

    void CalculateScatters()
    {
        uint32_t primeIdx = 0;
        m_allocScatter = g_Primes[primeIdx];
        while ((m_totalAllocOps % m_allocScatter) == 0)
            m_allocScatter = g_Primes[++primeIdx];

        m_freeScatter = g_Primes[++primeIdx];
        while ((m_totalAllocOps % m_freeScatter) == 0)
            m_freeScatter = g_Primes[++primeIdx];
    }

    int64_t GetRandomSize(Sizes bucket = Sizes::INVALID)
    {
        if (bucket == Sizes::INVALID) {
            auto rand = bm::utils::XorshiftNext(m_sizesState, 0.0f, 1.0f);
            if (rand < 0.7f)
                bucket = Sizes::SMALL;
            else if (rand < 0.9f)
                bucket = Sizes::MEDIUM;
            else
                bucket = Sizes::BIG;
        }

        switch (bucket) {
            case Sizes::SMALL:
                return g_smallSizes[bm::utils::XorshiftNext(
                    m_sizesState, 0, g_smallSizes.size() - 1)];
            case Sizes::MEDIUM:
                return g_mediumSizes[bm::utils::XorshiftNext(
                    m_sizesState, 0, g_mediumSizes.size() - 1)];
            case Sizes::BIG:
                return g_bigSizes[bm::utils::XorshiftNext(m_sizesState, 0, g_bigSizes.size() - 1)];
            case Sizes::COMBINED:
                return g_combinedSizes[bm::utils::XorshiftNext(
                    m_sizesState, 0, g_combinedSizes.size() - 1)];
            default:
                return 0;
        }
    }

    uint8_t NextMultipleAllocationsCount()
    {
        m_allocOpsIdx = (m_allocOpsIdx + m_allocScatter) % g_NumAllocOps.size();
        return m_allocOpsIdx;
    }

    uint8_t NextMultipleDellocationsCount()
    {
        m_freeOpsIdx = (m_freeOpsIdx + m_freeScatter) % g_NumFreeOps.size();
        return m_freeOpsIdx;
    }

    inline void AllocSingle(int64_t t_size)
    {
        if (m_totalAllocated >= m_maxMemoryConsumption /* || m_allocOps >= m_totalAllocOps */)
            return;

        if (m_activePtrs[m_allocIdx]) {
            m_totalAllocated -= *static_cast<int64_t*>(m_activePtrs[m_allocIdx]);
            BenchmarkDeallocate(m_activePtrs[m_allocIdx]);
            m_activePtrs[m_allocIdx] = nullptr;
            m_freeOps++;
        }

        m_activePtrs[m_allocIdx] = BenchmarkAllocate(t_size);
        *static_cast<int64_t*>(m_activePtrs[m_allocIdx]) = t_size;

        m_allocIdx = (m_allocIdx + m_allocScatter) % m_activePtrs.size();
        m_totalAllocated += t_size;

        ++m_allocOps;
    }

    inline void AllocMany(int32_t t_total, bool t_randomSizes = true)
    {
        if (m_totalAllocated >= m_maxMemoryConsumption /* || m_allocOps >= m_totalAllocOps */)
            return;

        // std::cout << "AllocMany(" << t_total << ", " << t_randomSizes << ")" << std::endl;
        for (uint32_t i = 0; i < t_total; i++) {
            int32_t randomSize = GetRandomSize();
            if (t_randomSizes)
                randomSize = GetRandomSize();

            AllocSingle(randomSize);
        }
    }

    inline void FreeSingle()
    {
        // if (m_freeOps >= m_totalFreeOps)
        //     return;

        // std::cout << "FreeSingle()" << std::endl;
        if (m_activePtrs[m_freeIdx]) {
            m_totalAllocated -= *static_cast<int64_t*>(m_activePtrs[m_freeIdx]);
            BenchmarkDeallocate(m_activePtrs[m_freeIdx]);
            m_activePtrs[m_freeIdx] = nullptr;
            ++m_freeOps;
        }

        m_freeIdx = (m_freeIdx + m_freeScatter) % m_activePtrs.size();
    }

    inline void FreeMany(int32_t t_total)
    {
        for (uint32_t i = 0; i < t_total; i++) {
            FreeSingle();
        }
    }
};

/**
 * @brief Benchmarks the allocation speed for many different small allocation requests.
 */
template<class... Args>
static void BM_Complex(benchmark::State& state, Args&&... args)
{
    auto argsTuple = std::make_tuple(std::move(args)...);
    auto totalOps = std::get<0>(argsTuple);
    auto totalAllocOps = std::get<1>(argsTuple);
    auto totalFreeOps = std::get<2>(argsTuple);
    auto transitionMatrix = std::get<3>(argsTuple);

    for (auto _ : state) {
        state.PauseTiming();
        Worker worker(totalOps, totalAllocOps, totalFreeOps, transitionMatrix);
        state.ResumeTiming();

        auto result = worker.RunBenchmark();

        state.PauseTiming();
        worker.CleanUp();
        state.counters["TotalOperations"] = std::get<0>(result);
        state.counters["TotalAllocOperations"] = std::get<1>(result);
        state.counters["TotalFreeOperations"] = std::get<2>(result);
        state.ResumeTiming();
    }
}

// BENCHMARK_CAPTURE(
//     BM_Complex,
//     "Total ops: 131072, Relative alloc ops: 0.8, Relative free ops: 0.2, Transition matrix:
//     ver-1", 2 << 16, (2 << 16) * 4 / 5, (2 << 16) * 1 / 5, std::array<std::array<float, 4>, 4>{
//         std::array<float, 4>{ 0.15, 0.0, 0.0, 0.85 },   // AllocateSingle
//         std::array<float, 4>{ 0.65, 0.0, 0.35, 0.0 },   // DeallocateSingle
//         std::array<float, 4>{ 0.0, 0.82, 0.0, 0.18 },   // AllocateMultiple
//         std::array<float, 4>{ 0.07, 0.00, 0.93, 0.00 }, // DeallocateMultiple
//     })
//     ->Unit(benchmark::kMillisecond);

// BENCHMARK_CAPTURE(
//     BM_Complex,
//     "Total ops: 131072, Relative alloc ops: 0.8, Relative free ops: 0.2, Transition matrix:
//     ver-2", (2 << 16), (2 << 16) * 4 / 5, (2 << 16) * 1 / 5, std::array<std::array<float, 4>, 4>{
//         std::array<float, 4>{ 0.2, 0.1, 0.6, 0.1 },   // AllocateSingle
//         std::array<float, 4>{ 0.4, 0.1, 0.3, 0.2 },   // DeallocateSingle
//         std::array<float, 4>{ 0.1, 0.4, 0.1, 0.4 },   // AllocateMultiple
//         std::array<float, 4>{ 0.5, 0.05, 0.4, 0.05 }, // DeallocateMultiple
//     })
//     ->Unit(benchmark::kMillisecond);

// BENCHMARK_CAPTURE(
//     BM_Complex,
//     "Total ops: 131072, Relative alloc ops: 0.6, Relative free ops: 0.4, Transition matrix:
//     ver-1", 2 << 16, (2 << 16) * 3 / 5, (2 << 16) * 2 / 5, std::array<std::array<float, 4>, 4>{
//         std::array<float, 4>{ 0.15, 0.0, 0.0, 0.85 },   // AllocateSingle
//         std::array<float, 4>{ 0.65, 0.0, 0.35, 0.0 },   // DeallocateSingle
//         std::array<float, 4>{ 0.0, 0.82, 0.0, 0.18 },   // AllocateMultiple
//         std::array<float, 4>{ 0.07, 0.00, 0.93, 0.00 }, // DeallocateMultiple
//     })
//     ->Unit(benchmark::kMillisecond);

// BENCHMARK_CAPTURE(
//     BM_Complex,
//     "Total ops: 131072, Relative alloc ops: 0.6, Relative free ops: 0.4, Transition matrix:
//     ver-2", (2 << 16), (2 << 16) * 3 / 5, (2 << 16) * 2 / 5, std::array<std::array<float, 4>, 4>{
//         std::array<float, 4>{ 0.2, 0.1, 0.6, 0.1 },   // AllocateSingle
//         std::array<float, 4>{ 0.4, 0.1, 0.3, 0.2 },   // DeallocateSingle
//         std::array<float, 4>{ 0.1, 0.4, 0.1, 0.4 },   // AllocateMultiple
//         std::array<float, 4>{ 0.5, 0.05, 0.4, 0.05 }, // DeallocateMultiple
//     })
//     ->Unit(benchmark::kMillisecond);

// BENCHMARK_CAPTURE(
//     BM_Complex,
//     "Total ops: 131072, Relative alloc ops: 0.4, Relative free ops: 0.6, Transition matrix:
//     ver-1", 2 << 16, (2 << 16) * 2 / 5, (2 << 16) * 3 / 5, std::array<std::array<float, 4>, 4>{
//         std::array<float, 4>{ 0.15, 0.0, 0.0, 0.85 },   // AllocateSingle
//         std::array<float, 4>{ 0.65, 0.0, 0.35, 0.0 },   // DeallocateSingle
//         std::array<float, 4>{ 0.0, 0.82, 0.0, 0.18 },   // AllocateMultiple
//         std::array<float, 4>{ 0.07, 0.00, 0.93, 0.00 }, // DeallocateMultiple
//     })
//     ->Unit(benchmark::kMillisecond);

// BENCHMARK_CAPTURE(
//     BM_Complex,
//     "Total ops: 131072, Relative alloc ops: 0.4, Relative free ops: 0.6, Transition matrix:
//     ver-2", (2 << 16), (2 << 16) * 2 / 5, (2 << 16) * 3 / 5, std::array<std::array<float, 4>, 4>{
//         std::array<float, 4>{ 0.2, 0.1, 0.6, 0.1 },   // AllocateSingle
//         std::array<float, 4>{ 0.4, 0.1, 0.3, 0.2 },   // DeallocateSingle
//         std::array<float, 4>{ 0.1, 0.4, 0.1, 0.4 },   // AllocateMultiple
//         std::array<float, 4>{ 0.5, 0.05, 0.4, 0.05 }, // DeallocateMultiple
//     })
//     ->Unit(benchmark::kMillisecond);

// BENCHMARK_CAPTURE(
//     BM_Complex,
//     "Total ops: 131072, Relative alloc ops: 0.2, Relative free ops: 0.8, Transition matrix:
//     ver-1", 2 << 16, (2 << 16) * 1 / 5, (2 << 16) * 4 / 5, std::array<std::array<float, 4>, 4>{
//         std::array<float, 4>{ 0.15, 0.0, 0.0, 0.85 },   // AllocateSingle
//         std::array<float, 4>{ 0.65, 0.0, 0.35, 0.0 },   // DeallocateSingle
//         std::array<float, 4>{ 0.0, 0.82, 0.0, 0.18 },   // AllocateMultiple
//         std::array<float, 4>{ 0.07, 0.00, 0.93, 0.00 }, // DeallocateMultiple
//     })
//     ->Unit(benchmark::kMillisecond);

// BENCHMARK_CAPTURE(
//     BM_Complex,
//     "Total ops: 131072, Relative alloc ops: 0.2, Relative free ops: 0.8, Transition matrix:
//     ver-2", (2 << 16), (2 << 16) * 1 / 5, (2 << 16) * 4 / 5, std::array<std::array<float, 4>, 4>{
//         std::array<float, 4>{ 0.2, 0.1, 0.6, 0.1 },   // AllocateSingle
//         std::array<float, 4>{ 0.4, 0.1, 0.3, 0.2 },   // DeallocateSingle
//         std::array<float, 4>{ 0.1, 0.4, 0.1, 0.4 },   // AllocateMultiple
//         std::array<float, 4>{ 0.5, 0.05, 0.4, 0.05 }, // DeallocateMultiple
//     })
//     ->Unit(benchmark::kMillisecond);

#define BENCHMARK_ITER(iters)                                                                      \
    BENCHMARK_CAPTURE(BM_Complex,                                                                  \
                      "Total ops: " #iters " , Relative alloc ops: 0.6, Relative free ops: 0.4, "  \
                      "Transition matrix: ver-2",                                                  \
                      (iters),                                                                     \
                      (iters)*1 / 2,                                                               \
                      (iters)*1 / 2,                                                               \
                      std::array<std::array<float, 4>, 4>{                                         \
                          std::array<float, 4>{ 0.15, 0.0, 0.0, 0.85 },   /* AllocateSingle */     \
                          std::array<float, 4>{ 0.65, 0.0, 0.35, 0.0 },   /* DeallocateSingle */   \
                          std::array<float, 4>{ 0.0, 0.82, 0.0, 0.18 },   /* AllocateMultiple */   \
                          std::array<float, 4>{ 0.07, 0.00, 0.93, 0.00 }, /* DeallocateMultiple */ \
                      })                                                                           \
        ->Unit(benchmark::kMillisecond);

BENCHMARK_ITER(100'000);
BENCHMARK_ITER(200'000);
BENCHMARK_ITER(300'000);
BENCHMARK_ITER(400'000);
BENCHMARK_ITER(500'000);
BENCHMARK_ITER(600'000);
BENCHMARK_ITER(700'000);
BENCHMARK_ITER(800'000);
BENCHMARK_ITER(900'000);
BENCHMARK_ITER(1'000'000);
BENCHMARK_ITER(1'100'000);
BENCHMARK_ITER(1'200'000);
BENCHMARK_ITER(1'300'000);
BENCHMARK_ITER(1'400'000);
BENCHMARK_ITER(1'500'000);
