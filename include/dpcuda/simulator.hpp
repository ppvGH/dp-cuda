#pragma once

#include "dpcuda/simulation_result.hpp"
#include "dpcuda/types.hpp"

#include <cstddef>

namespace dpcuda {

// Simulates one double pendulum using fixed-step RK4.
//
// steps is the number of integration intervals. The result contains
// steps + 1 samples, including the supplied initial state at t = 0.
//
// Throws std::invalid_argument for invalid state values, physical parameters,
// or timestep, and std::length_error when steps + 1 is not representable.
SimulationResult simulate(
    const State& initial_state,
    const Parameters& parameters,
    float dt,
    std::size_t steps
);

}  // namespace dpcuda

