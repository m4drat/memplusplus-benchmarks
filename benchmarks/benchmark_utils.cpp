#include "benchmark_utils.h"

namespace bm::utils {
    uint64_t XorshiftNext()
    {
        // PRNG state
        static uint64_t x{ 0xABCD133798765432 };

        uint64_t z = (x += UINT64_C(0x9E3779B97F4A7C15));
        z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
        z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
        return z ^ (z >> 31);
    }
}
