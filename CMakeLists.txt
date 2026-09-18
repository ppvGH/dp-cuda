cmake_minimum_required(VERSION 3.20)

project(
    DPCUDA
    VERSION 0.0.0
    DESCRIPTION "CUDA-accelerated double-pendulum simulator"
    LANGUAGES CXX
)

option(DP_CUDA_BUILD_TESTS "Build the test suite" ON)

add_executable(dp_cuda src/main.cpp)
set_target_properties(dp_cuda PROPERTIES OUTPUT_NAME "dp-cuda")
target_compile_features(dp_cuda PRIVATE cxx_std_17)
target_include_directories(
    dp_cuda
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(dp_cuda PRIVATE /W4 /permissive-)
else()
    target_compile_options(dp_cuda PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(DP_CUDA_BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()
