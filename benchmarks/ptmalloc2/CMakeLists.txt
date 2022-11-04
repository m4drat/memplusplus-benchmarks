project(benchmark-ptmalloc2)

# Create benchmark executable
add_executable(${PROJECT_NAME}
    allocator_api_override.cpp
    ${BENCHMARK_SOURCES}
)

# add_library(libc STATIC IMPORTED)
# include_directories(${PROJECT_SOURCE_DIR}/glibc-installation/include)
# set_target_properties(libc PROPERTIES IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/glibc-installation/lib/libc.a")
target_link_libraries(${PROJECT_NAME}
    benchmark::benchmark

    # libc
)

add_test(${PROJECT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})