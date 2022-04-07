project(benchmark-mempp)

# Add a testing executable
add_executable(${PROJECT_NAME} 
    bench_main.cpp
    benchmark_mpp.cpp
)

target_link_libraries(${PROJECT_NAME}
    lib::mpp
    benchmark::benchmark
)

# add_test(benchmark-mempp ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})