add_test(NAME dp_cuda_smoke COMMAND dp_cuda)
set_tests_properties(
    dp_cuda_smoke
    PROPERTIES PASS_REGULAR_EXPRESSION "DP-CUDA CPU project ready"
)

function(dpcuda_add_cpp_test test_name target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} PRIVATE dpcuda_core)

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive- /EHsc)
    else()
        target_compile_options(
            ${target_name}
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
        )
    endif()

    add_test(NAME ${test_name} COMMAND ${target_name})
endfunction()

dpcuda_add_cpp_test(
    dpcuda_dynamics
    dpcuda_dynamics_tests
    test_dynamics.cpp
)
dpcuda_add_cpp_test(
    dpcuda_energy
    dpcuda_energy_tests
    test_energy.cpp
)
dpcuda_add_cpp_test(
    dpcuda_rk4
    dpcuda_rk4_tests
    test_rk4.cpp
)
dpcuda_add_cpp_test(
    dpcuda_simulator
    dpcuda_simulator_tests
    test_simulator.cpp
)
