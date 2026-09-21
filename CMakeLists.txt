cmake_minimum_required(VERSION 3.20)

project(
    DPCUDA
    VERSION 0.0.0
    DESCRIPTION "CUDA-accelerated double-pendulum simulator"
    LANGUAGES CXX
)

option(DP_CUDA_BUILD_TESTS "Build the test suite" ON)
option(
    DP_CUDA_ENABLE_COVERAGE
    "Instrument GNU builds for source coverage"
    OFF
)

add_library(
    dpcuda_core
    src/dynamics.cpp
    src/energy.cpp
    src/simulator.cpp
)

target_compile_features(dpcuda_core PUBLIC cxx_std_17)

target_include_directories(
    dpcuda_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

if(DP_CUDA_ENABLE_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(FATAL_ERROR "Coverage instrumentation requires GNU C++")
    endif()

    # Coverage must propagate to every executable that links the instrumented
    # static library so that the gcov runtime is present at link time.
    target_compile_options(dpcuda_core PUBLIC --coverage -O0 -g)
    target_link_options(dpcuda_core PUBLIC --coverage)
endif()

add_executable(dp_cuda src/main.cpp)
set_target_properties(dp_cuda PROPERTIES OUTPUT_NAME "dp-cuda")

target_link_libraries(
    dp_cuda
    PRIVATE
        dpcuda_core
)

if(MSVC)
    target_compile_options(dpcuda_core PRIVATE /W4 /permissive-)
    target_compile_options(dp_cuda PRIVATE /W4 /permissive-)
else()
    target_compile_options(dpcuda_core PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(dp_cuda PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(DP_CUDA_BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()
