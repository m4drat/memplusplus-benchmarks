#include "allocator_api_override.h"
#include "benchmark/benchmark.h"
#include "benchmark_constants.h"
#include "benchmark_utils.h"
#include "memplusplus/libmemplusplus/include/mpplib/memory_manager.hpp"
#include "mpplib/memory_manager.hpp"
#include "mpplib/mpp.hpp"
#include "mpplib/utils/macros.hpp"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <tuple>

class WorkerGC
{
public:
    //! @brief Default transition matrix
    static constexpr std::array<std::array<float, 7>, 7> c_defaultTransitionMatrix{
        std::array<float,
                   7>{ 0.4040, 0.0200, 0.250, 0.020, 0.240, 0.0650, 0.0010 }, /* CREATE_VERTEX */
        std::array<float,
                   7>{ 0.1200, 0.4470, 0.150, 0.250, 0.020, 0.0090, 0.0040 }, /* REMOVE_VERTEX */
        std::array<float,
                   7>{ 0.1800, 0.1000, 0.380, 0.010, 0.300, 0.0290, 0.0010 }, /* CREATE_EDGE */
        std::array<float,
                   7>{ 0.1700, 0.3200, 0.127, 0.280, 0.043, 0.0530, 0.0070 }, /* REMOVE_EDGE */
        std::array<float,
                   7>{ 0.1016, 0.2090, 0.130, 0.150, 0.200, 0.2090, 0.0004 }, /* WRITE_DATA */
        std::array<float,
                   7>{ 0.1400, 0.1990, 0.110, 0.150, 0.200, 0.2000, 0.0010 }, /* READ_DATA  */
        std::array<float,
                   7>{ 0.3500, 0.0500, 0.350, 0.050, 0.099, 0.1009, 0.0001 }, /* COLLECT_GARBAGE */
    };

    static constexpr uint32_t c_maxPointersInAVertex = 32;

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
        CREATE_VERTEX = 0,
        REMOVE_VERTEX,
        CREATE_EDGE,
        REMOVE_EDGE,
        WRITE_DATA,
        READ_DATA,
        COLLECT_GARBAGE,
        COUNT,
        INVALID
    };

    class Vertex
    {
    private:
        std::array<mpp::SharedGcPtr<Vertex>, c_maxPointersInAVertex> m_gcs;
        uint64_t m_data;

    public:
        Vertex()
            : m_data(0x13371337deadbeef)
        {}

        void AddPointer(const mpp::SharedGcPtr<Vertex>& t_ptr)
        {
            auto vacant = std::find(m_gcs.begin(), m_gcs.end(), nullptr);

            if (vacant != m_gcs.end()) {
                *vacant = t_ptr;
            }
        }

        std::array<mpp::SharedGcPtr<Vertex>, c_maxPointersInAVertex>& GetPointers()
        {
            return m_gcs;
        }

        void RemovePointer()
        {
            auto pointerLoc =
                std::find_if(m_gcs.begin(), m_gcs.end(), [](const mpp::SharedGcPtr<Vertex>& t_ptr) {
                    return t_ptr != nullptr;
                });

            if (pointerLoc != m_gcs.end()) {
                *pointerLoc = nullptr;
            }
        }

        uint64_t& GetData()
        {
            return m_data;
        }
    };

    /**
     * @brief Construct a new WorkerGC object
     * @param t_bmState The benchmark state
     * @param t_totalOps The total number of loop iterations to perform
     * @param t_transitionMatrix The transition matrix
     * @param t_xorshiftSeed The seed for the xorshift random number generator
     */
    WorkerGC(benchmark::State& t_bmState,
             uint32_t t_totalOps = 1024 * 128,
             std::array<std::array<float, 7>, 7> t_transitionMatrix = c_defaultTransitionMatrix,
             uint64_t t_xorshiftSeed = 0x133796A5FF21B3C7)
        : m_bmState(t_bmState)
        , m_activePtrs(1024, nullptr)
        , m_totalOps(t_totalOps)
        , m_transitionMatrix(t_transitionMatrix)
    {
        m_transitionsRngState = bm::utils::XorshiftNext(t_xorshiftSeed);
        m_defaultRngState = bm::utils::XorshiftNext(t_xorshiftSeed);

        CheckTransitionMatrix();
        CreateRandomGraph();
    }

    struct BenchResults
    {
        //! @brief Total iterations count.
        uint32_t totalIterations;

        //! @brief Number of calls to GarbageCollect().
        uint32_t totalGcInvocations;

        //! @brief Number of active GC-Ptrs before benchmark exits.
        uint32_t totalActiveGcPtrs;
    };

    /**
     * @brief Runs the benchmark.
     * @return BenchResults- total number of iterations, number of allocations, number of
     * deallocations, max number of active pointers.
     */
    BenchResults RunBenchmark()
    {
        while (m_ops <= m_totalOps) {
            ++m_ops;
            switch (GetNextOperation()) {
                case Operation::CREATE_VERTEX: {
                    MPP_LOG_DBG("CREATE_VERTEX");
                    CreateVertexIfDoesntExist();
                    break;
                }
                case Operation::REMOVE_VERTEX: {
                    MPP_LOG_DBG("REMOVE_VERTEX");
                    m_activePtrs.at(GetActivePtrsRandIdx()).Reset();
                    break;
                }
                case Operation::CREATE_EDGE: {
                    MPP_LOG_DBG("CREATE_EDGE");
                    auto vtx_to_add = GetRandomActivePtr();
                    auto vtx_add_to = GetRandomActivePtr();

                    if (!vtx_to_add.has_value() || !vtx_add_to.has_value()) {
                        continue;
                    }

                    vtx_add_to.value().get()->AddPointer(vtx_to_add.value().get());

                    break;
                }
                case Operation::REMOVE_EDGE: {
                    MPP_LOG_DBG("REMOVE_EDGE");
                    auto vertex = GetRandomActivePtr();
                    if (!vertex.has_value()) {
                        continue;
                    }

                    vertex.value().get()->RemovePointer();
                    break;
                }
                case Operation::WRITE_DATA: {
                    MPP_LOG_DBG("WRITE_DATA");
                    auto vertex = GetRandomActivePtr();
                    if (!vertex.has_value()) {
                        continue;
                    }

                    vertex.value().get()->GetData() =
                        bm::utils::XorshiftNext(m_defaultRngState, (uint64_t)0, 0xdeadbeef);
                    break;
                }
                case Operation::READ_DATA: {
                    MPP_LOG_DBG("READ_DATA");
                    auto vertex = GetRandomActivePtr();
                    if (!vertex.has_value()) {
                        continue;
                    }

                    volatile auto data = vertex.value().get()->GetData();
                    break;
                }
                case Operation::COLLECT_GARBAGE: {
                    MPP_LOG_DBG("COLLECT_GARBAGE");
                    m_totalGcInvocations++;
                    mpp::CollectGarbage();
                    break;
                }
                default:
                    break;
            }
        }

        return BenchResults{ m_ops,
                             static_cast<uint32_t>(m_totalGcInvocations),
                             static_cast<uint32_t>(
                                 mpp::g_memoryManager->GetGC().GetGcPtrs().size()) };
    }

private:
    //! @brief Vector of pointers to allocated memory
    std::vector<mpp::SharedGcPtr<Vertex>> m_activePtrs;

    //! @brief Total operations to perform
    uint32_t m_totalOps;

    //! @brief Number of switch-cases executed
    uint32_t m_ops = 0;

    //! @brief Total number of GC invocations
    uint64_t m_totalGcInvocations = 0;

    //! @brief Random number generator state for transitions
    uint64_t m_transitionsRngState;

    //! @brief Default random number generator state
    uint64_t m_defaultRngState;

    //! @brief Google benchmark state
    benchmark::State& m_bmState;

    //! @brief Current statemachine state
    Operation m_currentOp = Operation::CREATE_VERTEX;

    //! @brief Transition matrix with probabilities of switching to another state
    std::array<std::array<float, 7>, 7> m_transitionMatrix;

    mpp::SharedGcPtr<Vertex>& CreateVertexAtIdxIfDoesntExist(uint32_t i)
    {
        if (m_activePtrs.at(i) == nullptr) {
            return CreateVertexAtIdx(i);
        }

        return m_activePtrs[i];
    }

    mpp::SharedGcPtr<Vertex>& CreateVertexAtIdx(uint32_t i)
    {
        auto vtx = mpp::MakeShared<Vertex>();
        m_activePtrs[i] = std::move(vtx);
        return m_activePtrs[i];
    }

    mpp::SharedGcPtr<Vertex>& CreateVertexIfDoesntExist()
    {
        uint32_t idx = GetActivePtrsRandIdx();

        if (m_activePtrs.at(idx) == nullptr) {
            return CreateVertexAtIdx(idx);
        }

        return m_activePtrs[idx];
    }

    mpp::SharedGcPtr<Vertex>& CreateVertex()
    {
        uint32_t idx = GetActivePtrsRandIdx();
        return CreateVertexAtIdx(idx);
    }

    std::optional<std::reference_wrapper<mpp::SharedGcPtr<Vertex>>> GetRandomActivePtr()
    {
        uint32_t vtxIdx1 = GetActivePtrsRandIdx();
        mpp::SharedGcPtr<Vertex>& vertex = m_activePtrs[vtxIdx1];
        if (!vertex) {
            auto loc = std::find_if(
                m_activePtrs.begin(),
                m_activePtrs.end(),
                [](const mpp::SharedGcPtr<Vertex>& t_ptr) { return t_ptr != nullptr; });

            if (loc == m_activePtrs.end()) {
                return {};
            }

            vertex = *loc;
        }

        return vertex;
    }

    inline uint32_t GetActivePtrsRandIdx()
    {
        return bm::utils::XorshiftNext(m_transitionsRngState, 0, m_activePtrs.size() - 1);
    }

    //! @brief Checks if transition matrix is valid
    void CheckTransitionMatrix()
    {
        for (uint32_t rowIdx = 0; rowIdx < m_transitionMatrix.size(); ++rowIdx) {
            float sum = 0.0f;
            for (auto& col : m_transitionMatrix[rowIdx]) {
                sum += col;
            }

            if (sum <= 0.99999f || sum >= 1.00001f) {
                throw std::runtime_error("Transition matrix row: " + std::to_string(rowIdx) +
                                         " is invalid. The sum is: " + std::to_string(sum));
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

    void CreateRandomGraph(const float t_graphDensity = 0.7,
                           const float t_vertexDensityFactor = 0.8,
                           const uint32_t t_totalSubgraphs = 9)
    {
        for (uint32_t clusterIdx = 0; clusterIdx < t_totalSubgraphs; clusterIdx++) {
            const uint32_t verticesInACluster = m_activePtrs.size() / t_totalSubgraphs;
            const uint32_t clusterStart = verticesInACluster * clusterIdx;
            const uint32_t clusterEnd = clusterStart + verticesInACluster;

            for (uint32_t i = clusterStart; i < clusterEnd; i++) {
                if (bm::utils::XorshiftNext(m_defaultRngState, 0.0f, 1.0f) > t_graphDensity) {
                    m_activePtrs[i] = std::move(mpp::MakeShared<Vertex>());
                    auto& vertexAdjList = m_activePtrs[i]->GetPointers();

                    for (uint32_t j = 0; j < vertexAdjList.size(); j++) {
                        if (bm::utils::XorshiftNext(m_defaultRngState, 0.0f, 1.0f) >
                            t_vertexDensityFactor) {
                            uint32_t otherVtxIdx = bm::utils::XorshiftNext(
                                m_defaultRngState, static_cast<uint64_t>(clusterStart), clusterEnd);
                            auto& otherVertex = CreateVertexAtIdxIfDoesntExist(otherVtxIdx);
                            vertexAdjList[j] = otherVertex;
                        }
                    }
                }
            }
        }

        // mpp::g_memoryManager->GetGC().DumpCurrentObjectsGraph(mpp::utils::ObjectsGraphDumpType::SIMPLE,
        // "generated-graph.dot");
    }
};

/**
 * @brief Benchmarks the performance of the allocator by performing random (but similar to real
 * program) sequences of operations.
 */
template<class... Args>
static void BM_ComplexGc(benchmark::State& state, Args&&... args)
{
    auto argsTuple = std::make_tuple(std::move(args)...);
    auto totalOps = std::get<0>(argsTuple);
    auto transitionMatrix = std::get<1>(argsTuple);

    WorkerGC::BenchResults result;
    for (auto _ : state) {
        BenchmarkAllocatorInitialize();
        WorkerGC workergc(state, totalOps, transitionMatrix);
        auto start = std::chrono::high_resolution_clock::now();
        result = workergc.RunBenchmark();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(duration.count());
    }

    state.counters["TotalControlLoopIterations"] = result.totalIterations;
    state.counters["TotalGcInvocations"] = result.totalGcInvocations;
    state.counters["TotalActiveGcPtrs"] = result.totalActiveGcPtrs;
    state.counters["PeakMemoryUsage"] = bm::utils::GetProcPeakMemoryUsage();
}

#define BENCHMARK_WITH_MATRIX(name, iters, transitions)                                            \
    BENCHMARK_CAPTURE(BM_ComplexGc, name, (iters), (transitions))                                  \
        ->Unit(benchmark::kMillisecond)                                                            \
        ->Iterations(7)                                                                            \
        ->UseManualTime()

#define BENCHMARK_SIMULATE_NORM_WORKLOAD(iters)                                                    \
    BENCHMARK_WITH_MATRIX(                                                                         \
        "Total ops: " #iters "Transition matrix: norm-workload",                                   \
        iters,                                                                                     \
        (std::array<std::array<float, 7>, 7>{                                                      \
            std::array<float, 7>{                                                                  \
                0.404, 0.02, 0.25, 0.02, 0.240, 0.065, 0.001 }, /* CREATE_VERTEX */                \
            std::array<float, 7>{                                                                  \
                0.12, 0.447, 0.15, 0.25, 0.020, 0.009, 0.004 }, /* REMOVE_VERTEX */                \
            std::array<float, 7>{                                                                  \
                0.18, 0.100, 0.38, 0.01, 0.300, 0.029, 0.001 }, /* CREATE_EDGE */                  \
            std::array<float, 7>{                                                                  \
                0.17, 0.32, 0.127, 0.28, 0.043, 0.053, 0.007 }, /* REMOVE_EDGE */                  \
            std::array<float, 7>{                                                                  \
                0.1016, 0.209, 0.13, 0.15, 0.20, 0.209, 0.0004 }, /* WRITE_DATA */                 \
            std::array<float, 7>{                                                                  \
                0.140, 0.1990, 0.110, 0.150, 0.20, 0.20, 0.001 }, /* READ_DATA */                  \
            std::array<float, 7>{                                                                  \
                0.35, 0.05, 0.35, 0.05, 0.099, 0.1009, 0.0001 }, /* COLLECT_GARBAGE */             \
        }))

#define BENCHMARK_SIMULATE_GC_HEAVY(iters)                                                         \
    BENCHMARK_WITH_MATRIX(                                                                         \
        "Total ops: " #iters "Transition matrix: gc-heavy",                                        \
        iters,                                                                                     \
        (std::array<std::array<float, 7>, 7>{                                                      \
            std::array<float, 7>{                                                                  \
                0.404, 0.02, 0.25, 0.02, 0.237, 0.060, 0.009 }, /* CREATE_VERTEX */                \
            std::array<float, 7>{                                                                  \
                0.12, 0.445, 0.15, 0.25, 0.020, 0.009, 0.006 }, /* REMOVE_VERTEX */                \
            std::array<float, 7>{                                                                  \
                0.18, 0.100, 0.38, 0.01, 0.297, 0.029, 0.004 },                 /* CREATE_EDGE */  \
            std::array<float, 7>{ 0.17, 0.29, 0.127, 0.25, 0.043, 0.05, 0.07 }, /* REMOVE_EDGE */  \
            std::array<float, 7>{                                                                  \
                0.1016, 0.209, 0.13, 0.15, 0.20, 0.209, 0.0004 }, /* WRITE_DATA */                 \
            std::array<float, 7>{                                                                  \
                0.140, 0.1990, 0.110, 0.150, 0.20, 0.20, 0.001 }, /* READ_DATA */                  \
            std::array<float, 7>{                                                                  \
                0.35, 0.05, 0.35, 0.05, 0.099, 0.1009, 0.0001 }, /* COLLECT_GARBAGE */             \
        }))                                                                                        \
        ->Iterations(5)

BENCHMARK_SIMULATE_NORM_WORKLOAD(50'000);
BENCHMARK_SIMULATE_NORM_WORKLOAD(100'000);

BENCHMARK_SIMULATE_GC_HEAVY(50'000);
BENCHMARK_SIMULATE_GC_HEAVY(100'000);