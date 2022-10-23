#include <cstdint>

namespace bm::utils {
    void XorshiftInit(uint64_t t_seed);
    uint64_t XorshiftNext(uint64_t& t_state);
    uint64_t XorshiftNext();
    uint64_t XorshiftNext(uint64_t& t_state, uint64_t t_min, uint64_t t_max);
    uint64_t XorshiftNext(uint64_t t_min, uint64_t t_max);
    float XorshiftNext(uint64_t& t_state, float t_min, float t_max);
}