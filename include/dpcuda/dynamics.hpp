#pragma once

#include "dpcuda/types.hpp"

namespace dpcuda {

// Evaluates the first-order equations of motion at the given state.
//
// State is reused as a four-component vector for the result:
// (dtheta1/dt, dtheta2/dt, domega1/dt, domega2/dt).
//
// This low-level operation assumes finite state components, positive masses
// and lengths, and finite non-negative gravity. Input validation belongs at
// the public simulation boundary rather than in each derivative evaluation.
State compute_state_derivative(
    const State& state,
    const Parameters& parameters
);

}  // namespace dpcuda
