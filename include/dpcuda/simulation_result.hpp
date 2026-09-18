#pragma once

#include "dpcuda/types.hpp"

#include <vector>

namespace dpcuda {

// Time samples and states of one trajectory.
//
// times and states always have the same size. Sample zero represents the
// supplied initial state at t = 0.
struct SimulationResult {
    std::vector<float> times;
    std::vector<State> states;
};

}  // namespace dpcuda

