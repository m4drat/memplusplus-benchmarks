project(benchmark-mimalloc)

add_subdirectory(mimalloc)

# Create benchmark executable
add_executable(${PROJECT_NAME}
    allocator_api_override.cpp
    ${BENCHMARK_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    mimalloc-static
    benchmark::benchmark
)

add_test(${PROJECT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})