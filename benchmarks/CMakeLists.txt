# add_subdirectory(gcpp)
list(APPEND BENCHMARK_SOURCES

    # ../benchmark_garbage_collection.cpp
    # ../benchmark_memory_access.cpp
    ../benchmark_main.cpp

    # ../benchmark_alloc.cpp
    # ../benchmark_dealloc.cpp
    # ../benchmark_alloc_dealloc.cpp
    ../benchmark_utils.cpp
    ../benchmark_complex.cpp)

if(MPP_BENCH_ONLY_MEMPLUSPLUS MATCHES "ON")
    add_subdirectory(mempp)
else()
    add_subdirectory(jemalloc)
    add_subdirectory(mempp)
    add_subdirectory(mimalloc)
    add_subdirectory(ptmalloc2)
    add_subdirectory(ptmalloc3)
    add_subdirectory(rpmalloc)
endif()
