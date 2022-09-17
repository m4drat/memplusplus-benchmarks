project(benchmark-mpp)

set(MPP_BUILD_TESTS OFF)
set(MPP_FULL_DEBUG OFF)
set(MPP_SECURE OFF)
set(MPP_PROFILE OFF)
set(MPP_STATS OFF)
set(MPP_BUILD_EXAMPLE OFF)
set(MPP_BUILD_SHARED_LIBS OFF)
add_subdirectory(memplusplus)

# Create benchmark executable
add_executable(${PROJECT_NAME}
    allocator_api_override.cpp
    ${BENCHMARK_SOURCES}
    benchmark_garbage_collection.cpp
    benchmark_memory_access.cpp
)

target_link_libraries(${PROJECT_NAME}
    lib::mpp
    benchmark::benchmark
)

add_test(${PROJECT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})