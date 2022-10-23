#include "benchmark_utils.h"
#include <limits>

namespace bm::utils {
    uint64_t g_xorshiftState{ 0x0 };

    void XorshiftInit(uint64_t t_seed)
    {
        g_xorshiftState = t_seed;
    }

    uint64_t XorshiftNext(uint64_t& t_state)
    {
        uint64_t z = (t_state += UINT64_C(0x9E3779B97F4A7C15));
        z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
        z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
        return z ^ (z >> 31);
    }

    uint64_t XorshiftNext()
    {
        return XorshiftNext(g_xorshiftState);
    }

    uint64_t XorshiftNext(uint64_t& t_state, uint64_t t_min, uint64_t t_max)
    {
        return XorshiftNext(t_state) % (t_max - t_min) + t_min;
    }

    uint64_t XorshiftNext(uint64_t t_min, uint64_t t_max)
    {
        return XorshiftNext(g_xorshiftState, t_min, t_max);
    }

    float XorshiftNext(uint64_t& t_state, float t_min, float t_max)
    {
        return XorshiftNext(t_state) / static_cast<float>(std::numeric_limits<uint64_t>::max()) *
                   (t_max - t_min) +
               t_min;
    }
}
