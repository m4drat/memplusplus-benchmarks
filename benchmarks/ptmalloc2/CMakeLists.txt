project(benchmark-ptmalloc2)

# Create benchmark executable
add_executable(${PROJECT_NAME} 
    allocator_api_override.cpp
    ../benchmark_main.cpp
    ../benchmark_alloc_dealloc.cpp
)

target_link_libraries(${PROJECT_NAME}
    benchmark::benchmark
)

add_test(${PROJECT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})