#include <iostream>

#include "dpcuda/types.hpp"

int main() {
    const dpcuda::State equilibrium{};
    const dpcuda::Parameters parameters{};

    std::cout << "DP-CUDA CPU project ready"
              << " (theta1=" << equilibrium.theta1
              << " rad, g=" << parameters.g << " m/s^2)\n";
    return 0;
}
