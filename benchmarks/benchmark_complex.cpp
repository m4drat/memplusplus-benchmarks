#include "allocator_api_override.h"
#include "benchmark/benchmark.h"
#include "benchmark_constants.h"
#include "benchmark_utils.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>

class Worker
{
public:
    //! @brief Enum with all possible allocation sizes.
    enum class Sizes
    {
        SMALL = 0,
        MEDIUM,
        BIG,
        COUNT,
        COMBINED,
        INVALID
    };

    //! @brief Enum with all possible operations.
    enum class Operation
    {
        ALLOC_SINGLE = 0,
        ALLOC_MANY,
        DEALLOC_SINGLE,
        DEALLOC_MANY,
        COUNT,
        INVALID
    };

    /**
     * @brief Construct a new Worker object
     * @param t_bmState The benchmark state
     * @param t_totalOps The total number of loop iterations to perform
     * @param t_transitionMatrix The transition matrix
     * @param t_maxMemoryConsumption Maximum allowed memory consumption (in bytes)
     * @param t_xorshiftSeed The seed for the xorshift random number generator
     */
    Worker(benchmark::State& t_bmState,
           uint32_t t_totalOps = 1024 * 128,
           std::array<std::array<float, 4>, 4> t_transitionMatrix = m_defaultTransitionMatrix,
           int64_t t_maxMemoryConsumption = (int64_t)1024 * 1024 * 1024 * 2 /* 1024 Mb */,
           uint64_t t_xorshiftSeed = 0x133796A5FF21B3C1)
        : m_bmState(t_bmState)
        , m_activePtrs(20'000, nullptr)
        , m_totalOps(t_totalOps)
        , m_maxMemoryConsumption(t_maxMemoryConsumption)
        , m_transitionMatrix(t_transitionMatrix)
    {
        m_transitionsRngState = bm::utils::XorshiftNext(t_xorshiftSeed);
        m_sizesRngState = bm::utils::XorshiftNext(t_xorshiftSeed);

        CheckTransitionMatrix();
        CalculateScatters();
    }

    /**
     * @brief Runs the benchmark.
     * @return std::tuple<uint32_t, uint32_t, uint32_t, int32_t> - total number of iterations,
     * number of allocations, number of deallocations, max number of active pointers.
     */
    std::tuple<uint32_t, uint32_t, uint32_t, int32_t> RunBenchmark()
    {
        m_bmState.PauseTiming();

        while (m_allocOps + m_freeOps <= m_totalOps) {
            ++m_ops;
            auto op = GetNextOperation();
            switch (op) {
                case Operation::ALLOC_SINGLE:
                    // std::cout << m_ops << ". Alloc single" << std::endl;
                    AllocSingle(GetRandomSize());
                    break;
                case Operation::ALLOC_MANY:
                    // std::cout << m_ops << ". Alloc many" << std::endl;
                    AllocMany(NextMultipleAllocationsCount(),
                              (bm::utils::XorshiftNext(m_sizesRngState, 0.0f, 1.0f) > 0.8) ? true
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
        }

        return { m_ops, m_allocOps, m_freeOps, m_maxActivePtrs };
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
    //! @brief Vector of pointers to allocated memory
    std::vector<void*> m_activePtrs;

    //! @brief Total operations to perform
    uint32_t m_totalOps;

    //! @brief Number of alloc operations performed
    uint32_t m_allocOps = 0;

    //! @brief Number of free operations performed
    uint32_t m_freeOps = 0;

    //! @brief Number of switch-cases executed
    uint32_t m_ops = 0;

    //! @brief Maximum number of active pointers
    int32_t m_maxActivePtrs = std::numeric_limits<int32_t>::min();

    //! @brief Current number of active pointers
    int32_t m_currActivePtrs = 0;

    //! @brief Maximum allowed memory consumption
    int64_t m_maxMemoryConsumption;
    //! @brief Current memory consumption
    int64_t m_totalAllocated = 0;

    //! @brief Alloc scatter factor
    uint32_t m_allocScatter;
    //! @brief Free scatter factor
    uint32_t m_freeScatter;

    //! @brief Random number generator state for transitions
    uint64_t m_transitionsRngState;
    //! @brief Random number generator state for sizes generation
    uint64_t m_sizesRngState;

    //! @brief Next pointer index inside m_activePtrs to free
    uint32_t m_freeIdx = 0;
    //! @brief Index inside m_activePtrs where to save newly allocated chunk
    uint32_t m_allocIdx = 0;
    //! @brief Index inside g_NumAllocOps to get number of sequential allocations to perform
    uint64_t m_allocOpsIdx = 0;
    //! @brief Index inside g_NumAllocOps to get number of sequential deallocations to perform
    uint64_t m_freeOpsIdx = 0;

    //! @brief Google benchmark state
    benchmark::State& m_bmState;

    //! @brief Current statemachine state
    Operation m_currentOp = Operation::ALLOC_SINGLE;
    //! @brief Transition matrix with probabilities of switching to another state
    std::array<std::array<float, 4>, 4> m_transitionMatrix;
    //! @brief Default transition matrix
    static constexpr std::array<std::array<float, 4>, 4> m_defaultTransitionMatrix{
        std::array<float, 4>{ 0.15, 0.0, 0.0, 0.85 },   // AllocateSingle
        std::array<float, 4>{ 0.65, 0.0, 0.35, 0.0 },   // DeallocateSingle
        std::array<float, 4>{ 0.0, 0.82, 0.0, 0.18 },   // AllocateMultiple
        std::array<float, 4>{ 0.07, 0.00, 0.93, 0.00 }, // DeallocateMultiple
    };

    //! @brief Checks if transition matrix is valid
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

    //! @brief Performs transition to another state
    Operation GetNextOperation()
    {
        auto& row = m_transitionMatrix[static_cast<uint32_t>(m_currentOp)];
        auto rand = bm::utils::XorshiftNext(m_transitionsRngState, 0.0f, 1.0f);
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

    //! @brief Calculates alloc and free scatter factors
    void CalculateScatters()
    {
        uint32_t primeIdx = 0;
        m_allocScatter = g_Primes[primeIdx];
        while ((m_activePtrs.size() % m_allocScatter) == 0)
            m_allocScatter = g_Primes[++primeIdx];

        m_freeScatter = g_Primes[++primeIdx];
        while ((m_activePtrs.size() % m_freeScatter) == 0)
            m_freeScatter = g_Primes[++primeIdx];
    }

    /**
     * @brief Gets random size for allocation. If no argument is passed, size is generated randomly.
     * With probability of 0.7 size is generated from g_smallSizes, with probability of 0.2 size is
     * generated from g_mediumSizes, with probability of 0.1 size is generated from g_bigSizes.
     * @param bucket Bucket to get size from. @sa Sizes
     * @return int64_t Random size
     */
    int64_t GetRandomSize(Sizes bucket = Sizes::INVALID)
    {
        if (bucket == Sizes::INVALID) {
            auto rand = bm::utils::XorshiftNext(m_sizesRngState, 0.0f, 1.0f);
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
                    m_sizesRngState, 0, g_smallSizes.size() - 1)];
            case Sizes::MEDIUM:
                return g_mediumSizes[bm::utils::XorshiftNext(
                    m_sizesRngState, 0, g_mediumSizes.size() - 1)];
            case Sizes::BIG:
                return g_bigSizes[bm::utils::XorshiftNext(
                    m_sizesRngState, 0, g_bigSizes.size() - 1)];
            case Sizes::COMBINED:
                return g_combinedSizes[bm::utils::XorshiftNext(
                    m_sizesRngState, 0, g_combinedSizes.size() - 1)];
            default:
                return 0;
        }
    }

    //! @brief Next number of sequential allocations to perform
    uint8_t NextMultipleAllocationsCount()
    {
        m_allocOpsIdx = (m_allocOpsIdx + m_allocScatter) % g_NumAllocOps.size();
        return m_allocOpsIdx;
    }

    //! @brief Next number of sequential deallocations to perform
    uint8_t NextMultipleDellocationsCount()
    {
        m_freeOpsIdx = (m_freeOpsIdx + m_freeScatter) % g_NumFreeOps.size();
        return m_freeOpsIdx;
    }

    /**
     * @brief Allocates single chunk.
     * @param t_size Size of chunk to allocate
     */
    inline void AllocSingle(int64_t t_size)
    {
        if (m_totalAllocated >= m_maxMemoryConsumption)
            return;

        m_bmState.ResumeTiming();

        if (m_activePtrs[m_allocIdx]) {
            m_totalAllocated -= *static_cast<int64_t*>(m_activePtrs[m_allocIdx]);
            BenchmarkDeallocate(m_activePtrs[m_allocIdx]);
            m_activePtrs[m_allocIdx] = nullptr;
            ++m_freeOps;
            --m_currActivePtrs;
        }

        m_activePtrs[m_allocIdx] = BenchmarkAllocate(t_size);
        m_bmState.PauseTiming();
        *static_cast<int64_t*>(m_activePtrs[m_allocIdx]) = t_size;

        m_allocIdx = (m_allocIdx + m_allocScatter) % m_activePtrs.size();
        m_totalAllocated += t_size;

        ++m_allocOps;
        ++m_currActivePtrs;

        if (m_currActivePtrs > m_maxActivePtrs)
            m_maxActivePtrs = m_currActivePtrs;
    }

    /**
     * @brief Allocates multiple chunks.
     * @param t_totalChunks Total number of chunks to allocate.
     * @param t_randomSizes If true, sizes are generated per-chunk randomly.
     * Otherwise, all chunks are of the same size.
     */
    inline void AllocMany(int32_t t_totalChunks, bool t_randomSizes = true)
    {
        if (m_totalAllocated >= m_maxMemoryConsumption)
            return;

        // std::cout << "AllocMany(" << t_total << ", " << t_randomSizes << ")" << std::endl;
        for (uint32_t i = 0; i < t_totalChunks; i++) {
            int32_t randomSize = GetRandomSize();
            if (t_randomSizes)
                randomSize = GetRandomSize();

            AllocSingle(randomSize);
        }
    }

    //! @brief Frees single chunk.
    inline void FreeSingle()
    {
        if (m_activePtrs[m_freeIdx]) {
            m_totalAllocated -= *static_cast<int64_t*>(m_activePtrs[m_freeIdx]);
            m_bmState.ResumeTiming();
            BenchmarkDeallocate(m_activePtrs[m_freeIdx]);
            m_bmState.PauseTiming();
            m_activePtrs[m_freeIdx] = nullptr;
            ++m_freeOps;
            --m_currActivePtrs;
        }

        m_freeIdx = (m_freeIdx + m_freeScatter) % m_activePtrs.size();
    }

    /**
     * @brief Frees multiple chunks.
     * @param t_total Total number of chunks to free.
     */
    inline void FreeMany(int32_t t_total)
    {
        for (uint32_t i = 0; i < t_total; i++) {
            FreeSingle();
        }
    }
};

/**
 * @brief Benchmarks the performance of the allocator by performing random (but similar to real
 * program) sequences of  allocations and deallocations
 */
template<class... Args>
static void BM_Complex(benchmark::State& state, Args&&... args)
{
    auto argsTuple = std::make_tuple(std::move(args)...);
    auto totalOps = std::get<0>(argsTuple);
    auto transitionMatrix = std::get<1>(argsTuple);

    for (auto _ : state) {
        state.PauseTiming();
        Worker worker(state, totalOps, transitionMatrix);
        state.ResumeTiming();

        auto result = worker.RunBenchmark();

        state.PauseTiming();
        worker.CleanUp();
        state.counters["TotalOperations"] = std::get<0>(result);
        state.counters["TotalAllocOperations"] = std::get<1>(result);
        state.counters["TotalFreeOperations"] = std::get<2>(result);
        state.counters["MaxActivePtrs"] = std::get<3>(result);
        state.ResumeTiming();
    }
}

#define BENCHMARK_MAT1(iters)                                                                      \
    BENCHMARK_CAPTURE(BM_Complex,                                                                  \
                      "Total ops: " #iters "Transition matrix: ver-1",                             \
                      (iters),                                                                     \
                      std::array<std::array<float, 4>, 4>{                                         \
                          std::array<float, 4>{ 0.2, 0.1, 0.6, 0.1 },   /* AllocateSingle */       \
                          std::array<float, 4>{ 0.4, 0.1, 0.3, 0.2 },   /* DeallocateSingle */     \
                          std::array<float, 4>{ 0.1, 0.4, 0.1, 0.4 },   /* AllocateMultiple */     \
                          std::array<float, 4>{ 0.5, 0.05, 0.4, 0.05 }, /* DeallocateMultiple */   \
                      })                                                                           \
        ->Unit(benchmark::kMillisecond);

#define BENCHMARK_MAT2(iters)                                                                      \
    BENCHMARK_CAPTURE(BM_Complex,                                                                  \
                      "Total ops: " #iters "Transition matrix: ver-2",                             \
                      (iters),                                                                     \
                      std::array<std::array<float, 4>, 4>{                                         \
                          std::array<float, 4>{ 0.15, 0.0, 0.0, 0.85 },   /* AllocateSingle */     \
                          std::array<float, 4>{ 0.65, 0.0, 0.35, 0.0 },   /* DeallocateSingle */   \
                          std::array<float, 4>{ 0.0, 0.82, 0.0, 0.18 },   /* AllocateMultiple */   \
                          std::array<float, 4>{ 0.07, 0.00, 0.93, 0.00 }, /* DeallocateMultiple */ \
                      })                                                                           \
        ->Unit(benchmark::kMillisecond);

BENCHMARK_MAT1(200'000);
BENCHMARK_MAT1(400'000);
BENCHMARK_MAT1(600'000);
BENCHMARK_MAT1(800'000);
BENCHMARK_MAT1(1'000'000);
BENCHMARK_MAT1(1'200'000);
BENCHMARK_MAT1(1'400'000);
BENCHMARK_MAT1(1'600'000);
BENCHMARK_MAT1(1'800'000);
BENCHMARK_MAT1(2'000'000);