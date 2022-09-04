project(benchmark-jemalloc)

include(ExternalProject)

ExternalProject_Add(jemalloc-builder

    # --Configure step-------------
    SOURCE_DIR "${PROJECT_SOURCE_DIR}/jemalloc/"
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env "CC=${CMAKE_C_COMPILER}" "CXX=${CMAKE_CXX_COMPILER}" ./autogen.sh --enable-static --prefix=${PROJECT_SOURCE_DIR}/jemalloc-installation
    CMAKE_COMMAND ""

    # --Build step-----------------
    BUILD_COMMAND make -j8 install
    BUILD_IN_SOURCE 1

    # --Install step--------------
    INSTALL_COMMAND ""
)
include_directories(jemalloc-installation/include)

add_library(libjemalloc STATIC IMPORTED)
set_target_properties(libjemalloc PROPERTIES IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/jemalloc-installation/lib/libjemalloc.a")

# Create benchmark executable
add_executable(${PROJECT_NAME}
    allocator_api_override.cpp
    ../benchmark_main.cpp
    ../benchmark_alloc_dealloc.cpp
)

target_link_libraries(${PROJECT_NAME}
    benchmark::benchmark
    libjemalloc
    dl
)

add_dependencies(${PROJECT_NAME} jemalloc-builder)
add_test(${PROJECT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})