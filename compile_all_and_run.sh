#!/bin/bash
CXX=/usr/bin/clang++-15 CC=/usr/bin/clang-15 cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/clang-15 -DCMAKE_CXX_COMPILER=/usr/bin/clang++-15
CXX=/usr/bin/clang++-15 CC=/usr/bin/clang-15 cmake --build build --target all -- -j 16

# Benchmarks default allocation/deallocation speed
./build/benchmarks/mempp/benchmark-mpp
./build/benchmarks/ptmalloc2/benchmark-ptmalloc2
./build/benchmarks/ptmalloc3/benchmark-ptmalloc3
./build/benchmarks/mimalloc/benchmark-mimalloc
./build/benchmarks/rpmalloc/benchmark-rpmalloc

# Benchmark elements access before layouting and after