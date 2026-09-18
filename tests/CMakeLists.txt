add_test(NAME dp_cuda_smoke COMMAND dp_cuda)
set_tests_properties(
    dp_cuda_smoke
    PROPERTIES PASS_REGULAR_EXPRESSION "DP-CUDA CPU project ready"
)
