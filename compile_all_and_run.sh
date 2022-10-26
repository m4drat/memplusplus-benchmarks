#!/bin/bash
CXX=/usr/bin/clang++-15 CC=/usr/bin/clang-15 cmake -S . -B build -DMPP_BENCH_ONLY_MEMPLUSPLUS=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=On -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/clang-15 -DCMAKE_CXX_COMPILER=/usr/bin/clang++-15
CXX=/usr/bin/clang++-15 CC=/usr/bin/clang-15 cmake --build build --target all -- -j 16

# Benchmarks default allocation/deallocation speed
# ./build/benchmarks/gcpp/benchmark-gcpp

curr_date=$(date -u +"%Y-%m-%dT-%H_%M_%SZ")

mkdir -p ./bench-results/$curr_date

./build/benchmarks/jemalloc/benchmark-jemalloc --benchmark_out_format=json --benchmark_out="./bench-results/$curr_date/jemalloc.json" --benchmark_repetitions=5 
./build/benchmarks/mempp/benchmark-mpp --benchmark_out_format=json --benchmark_out="./bench-results/$curr_date/mpp.json" --benchmark_repetitions=5
./build/benchmarks/ptmalloc2/benchmark-ptmalloc2 --benchmark_out_format=json --benchmark_out="./bench-results/$curr_date/ptmalloc2.json" --benchmark_repetitions=5
./build/benchmarks/ptmalloc3/benchmark-ptmalloc3 --benchmark_out_format=json --benchmark_out="./bench-results/$curr_date/ptmalloc3.json" --benchmark_repetitions=5
./build/benchmarks/rpmalloc/benchmark-rpmalloc --benchmark_out_format=json --benchmark_out="./bench-results/$curr_date/rpmalloc.json" --benchmark_repetitions=5
./build/benchmarks/mimalloc/benchmark-mimalloc --benchmark_out_format=json --benchmark_out="./bench-results/$curr_date/mimalloc.json" --benchmark_repetitions=5
