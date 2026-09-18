add_test(NAME dp_cuda_smoke COMMAND dp_cuda)
set_tests_properties(
    dp_cuda_smoke
    PROPERTIES PASS_REGULAR_EXPRESSION "DP-CUDA CPU project ready"
)

add_executable(dpcuda_dynamics_tests test_dynamics.cpp)
target_link_libraries(dpcuda_dynamics_tests PRIVATE dpcuda_core)

if(MSVC)
    target_compile_options(dpcuda_dynamics_tests PRIVATE /W4 /permissive-)
else()
    target_compile_options(
        dpcuda_dynamics_tests
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()

add_test(NAME dpcuda_dynamics COMMAND dpcuda_dynamics_tests)

add_executable(dpcuda_rk4_tests test_rk4.cpp)
target_link_libraries(dpcuda_rk4_tests PRIVATE dpcuda_core)

if(MSVC)
    target_compile_options(dpcuda_rk4_tests PRIVATE /W4 /permissive-)
else()
    target_compile_options(
        dpcuda_rk4_tests
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()

add_test(NAME dpcuda_rk4 COMMAND dpcuda_rk4_tests)

add_executable(dpcuda_simulator_tests test_simulator.cpp)
target_link_libraries(dpcuda_simulator_tests PRIVATE dpcuda_core)

if(MSVC)
    target_compile_options(dpcuda_simulator_tests PRIVATE /W4 /permissive-)
else()
    target_compile_options(
        dpcuda_simulator_tests
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()

add_test(NAME dpcuda_simulator COMMAND dpcuda_simulator_tests)
