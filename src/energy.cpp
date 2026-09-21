#include "dpcuda/energy.hpp"

#include <cmath>

namespace dpcuda {

Energy compute_energy(
    const State& state,
    const Parameters& parameters
) {
    const float delta = state.theta1 - state.theta2;

    const float kinetic =
        0.5F
            * (parameters.m1 + parameters.m2)
            * parameters.l1
            * parameters.l1
            * state.omega1
            * state.omega1
        + 0.5F
            * parameters.m2
            * parameters.l2
            * parameters.l2
            * state.omega2
            * state.omega2
        + parameters.m2
            * parameters.l1
            * parameters.l2
            * state.omega1
            * state.omega2
            * std::cos(delta);

    const float potential =
        (parameters.m1 + parameters.m2)
            * parameters.g
            * parameters.l1
            * (1.0F - std::cos(state.theta1))
        + parameters.m2
            * parameters.g
            * parameters.l2
            * (1.0F - std::cos(state.theta2));

    return {
        kinetic,
        potential,
        kinetic + potential
    };
}

}  // namespace dpcuda
