project(benchmark-rpmalloc)

include_directories(rpmalloc/rpmalloc)

# Create benchmark executable
add_executable(${PROJECT_NAME}
    rpmalloc/rpmalloc/rpmalloc.c
    allocator_api_override.cpp
    ${BENCHMARK_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    benchmark::benchmark
)

add_test(${PROJECT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})