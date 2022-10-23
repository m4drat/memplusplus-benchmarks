# memplusplus-benchmarks

## What is it

This repo provides a set of different benchmarks for [memplusplus](https://github.com/m4drat/memplusplus/) memory allocator. Each test benchmarks a wide range of functionality starting from simple allocation/deallocation speed and ending with some complex things (such as speed of data access after heap compacting/relayouting).

### How to run

1. Clone this repo: `git clone https://github.com/m4drat/memplusplus-benchmarks --recurse-submodules`
2. Run all benchmarks `cd memplusplus-benchmarks && ./compile_all_and_run.sh.sh`

### Benchmarks description

1. `benchmark_alloc.cpp` - Sequence of allocations from the same size bucket
2. `benchmark_alloc_dealloc.cpp` - Allocations and immediate deallocations
3. `benchmark_dealloc.cpp` - Sequence of chunks deallocations from the same size bucket
4. Data access speed before compacting/layouting (based on P2329-move_at_scale)
5. Data access speed after compacting/layouting (based on P2329-move_at_scale)

### Targets

- [x] gcpp
- [x] jemalloc
- [x] mempp
- [x] mimalloc
- [?] ptmalloc2 - latest (glibc 2.35), currently uses `libc 2.31`
- [x] ptmalloc3
- [x] rpmalloc
