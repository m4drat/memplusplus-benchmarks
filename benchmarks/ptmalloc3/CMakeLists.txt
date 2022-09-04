project(benchmark-ptmalloc3)

include(ExternalProject)

ExternalProject_Add(libptmalloc3-builder

    # --Configure step-------------
    SOURCE_DIR "${PROJECT_SOURCE_DIR}/ptmalloc3/"
    CONFIGURE_COMMAND ""
    CMAKE_COMMAND ""

    # --Build step-----------------
    BUILD_COMMAND make all
    BUILD_IN_SOURCE 1

    # --Install step--------------
    INSTALL_COMMAND ""
)
include_directories(ptmalloc3)

add_library(libptmalloc3 STATIC IMPORTED)
set_target_properties(libptmalloc3 PROPERTIES IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/ptmalloc3/libptmalloc3.a")

# Create benchmark executable
add_executable(${PROJECT_NAME}
    allocator_api_override.cpp
    ../benchmark_main.cpp
    ../benchmark_alloc_dealloc.cpp
)

target_link_libraries(${PROJECT_NAME}
    benchmark::benchmark
    libptmalloc3
)
add_dependencies(${PROJECT_NAME} libptmalloc3-builder)

add_test(${PROJECT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})