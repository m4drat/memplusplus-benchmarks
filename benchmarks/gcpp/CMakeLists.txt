project(benchmark-gcpp)

add_subdirectory(gcpp)
include_directories(gcpp)
include_directories(gcpp/submodules/gsl/include)

# Create benchmark executable
add_executable(${PROJECT_NAME}
    allocator_api_override.cpp
    ${BENCHMARK_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    benchmark::benchmark
)

add_test(${PROJECT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})