cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)

project(memplusplus-benchmarks)

include_directories("libraries/benchmark")
include_directories("benchmarks")

add_subdirectory(libraries/googletest)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Disable google benchmark tests" FORCE)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable google benchmark tests" FORCE)
add_subdirectory(libraries/benchmark)

add_subdirectory(benchmarks)
