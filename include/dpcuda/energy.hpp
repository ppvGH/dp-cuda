#pragma once

#include "dpcuda/types.hpp"

namespace dpcuda {

// Mechanical energy of one double-pendulum state, expressed in joules.
//
// Potential is measured relative to the stable hanging configuration.
// Total is the sum of kinetic and potential.
struct Energy {
    float kinetic{};
    float potential{};
    float total{};
};

// Evaluates the mechanical energy of one double-pendulum state.
//
// This low-level operation assumes finite state components, positive finite
// masses and lengths, and finite non-negative gravity.
Energy compute_energy(
    const State& state,
    const Parameters& parameters
);

}  // namespace dpcuda
