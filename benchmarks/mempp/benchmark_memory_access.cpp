#include "allocator_api_override.h"
#include "benchmark/benchmark.h"
#include "benchmark_constants.h"
#include "benchmark_utils.h"
#include "mpplib/memory_manager.hpp"
#include "mpplib/mpp.hpp"

#include <algorithm>
#include <cstdlib>
#include <random>

using namespace mpp;

template<bool RandomizedLinkedList, bool DoLayout = false>
class Worker
{
public:
    Worker(benchmark::State& t_bmState,
           uint32_t t_LinkedListSize,
           uint64_t t_xorshiftSeed = 0x133796A5FF21B3C1)
        : m_bmState(t_bmState)
        , m_LinkedListSize(t_LinkedListSize)
        , m_xorshiftSeed(t_xorshiftSeed)
    {
        if constexpr (RandomizedLinkedList) {
            m_LinkedListHead = CreateRandomizedLinkedList(t_LinkedListSize);
        } else {
            m_LinkedListHead = CreateLayoutedLinkedList(t_LinkedListSize);
        }

        if constexpr (DoLayout) {
            GC::GetInstance().Collect();
        }
    }

    Worker(const Worker&) = delete;
    Worker(Worker&&) = delete;
    Worker& operator=(const Worker&) = delete;
    Worker& operator=(Worker&&) = delete;
    ~Worker() = default;

    void DoCleanup()
    {
        m_LinkedListHead = nullptr;
    }

    uint32_t DoBenchmark()
    {
        auto current = m_LinkedListHead;

        while (current->next != nullptr) {
            current->data = current->data ^ current->next->data ^ 0x1337AF12;
            current = current->next;
        }

        return current->data;
    }

private:
    struct alignas(64) ListNode
    {
        uint32_t index;
        uint32_t data;
        SharedGcPtr<ListNode> next;

        ListNode(uint32_t t_index, uint32_t t_data)
            : index(t_index)
            , data(t_data)
            , next(nullptr)
        {}
    };

    uint64_t m_xorshiftSeed;
    benchmark::State& m_bmState;
    SharedGcPtr<ListNode> m_LinkedListHead;
    uint32_t m_LinkedListSize;

    SharedGcPtr<ListNode> CreateLayoutedLinkedList(uint32_t size)
    {
        uint32_t data = 0xF7ADF3E1;
        SharedGcPtr<ListNode> head = MakeShared<ListNode>(0, data);
        SharedGcPtr<ListNode> current = head;

        for (uint32_t i = 1; i < size; ++i) {
            data = (data + 0xffffd) % ((2 << 20) + 1);
            current->next = MakeShared<ListNode>(i, data);
            current = current->next;
        }

        return head;
    }

    SharedGcPtr<ListNode> CreateRandomizedLinkedList(uint32_t size)
    {
        std::vector<SharedGcPtr<ListNode>> nodes;
        nodes.reserve(size);
        uint32_t data = 0xF7ADF3E1;

        for (uint32_t i = 0; i < size; ++i) {
            data = (data + 0xffffd) % ((2 << 20) + 1);
            nodes.emplace_back(MakeShared<ListNode>(i, data));
        }

        std::shuffle(
            std::begin(nodes), std::end(nodes), std::default_random_engine(m_xorshiftSeed));

        for (uint32_t i = 0; i < size - 1; ++i) {
            nodes[i]->next = nodes[i + 1];
        }

        return nodes[0];
    }
};

#define BENCHMARK_MEM_ACCESS(BM_NAME, RANDOMIZED_LINKED_LIST, DO_LAYOUT)                           \
    static void BM_NAME(benchmark::State& state)                                                   \
    {                                                                                              \
        for (auto _ : state) {                                                                     \
            state.PauseTiming();                                                                   \
            mpp::MM::ResetAllocatorState();                                                        \
            Worker<RANDOMIZED_LINKED_LIST, DO_LAYOUT> worker(state, state.range(0));               \
            state.ResumeTiming();                                                                  \
            uint32_t tmp = worker.DoBenchmark();                                                   \
            benchmark::DoNotOptimize(tmp);                                                         \
            benchmark::ClobberMemory();                                                            \
        }                                                                                          \
    }                                                                                              \
    BENCHMARK(BM_NAME)                                                                             \
        ->RangeMultiplier(2)                                                                       \
        ->Range(g_accessMemoryRangeStart, g_accessMemoryRangeEnd)                                  \
        ->Unit(benchmark::kMicrosecond);

BENCHMARK_MEM_ACCESS(BM_AccessMemoryDefaultLinkedList, false, false);
BENCHMARK_MEM_ACCESS(BM_AccessMemoryRandomizedLinkedList, true, false);

// Perform layouting
BENCHMARK_MEM_ACCESS(BM_AccessMemoryDefaultLayoutedLinkedList, false, true);
BENCHMARK_MEM_ACCESS(BM_AccessMemoryRandomizedLayoutedLinkedList, true, true);
