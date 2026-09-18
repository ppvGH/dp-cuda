#include "dpcuda/dynamics.hpp"

#include <cmath>

namespace dpcuda {

State compute_state_derivative(
    const State& state,
    const Parameters& parameters
) {
    const float delta = state.theta1 - state.theta2;
    const float sin_delta = std::sin(delta);
    const float cos_delta = std::cos(delta);

    const float total_mass = parameters.m1 + parameters.m2;

    // Coefficient matrix for the coupled angular-acceleration equations:
    //
    // a * alpha1 + b * alpha2 = rhs1
    // c * alpha1 + d * alpha2 = rhs2
    const float a = total_mass * parameters.l1;
    const float b = parameters.m2 * parameters.l2 * cos_delta;
    const float c = parameters.l1 * cos_delta;
    const float d = parameters.l2;

    const float rhs1 =
        -parameters.m2
            * parameters.l2
            * state.omega2
            * state.omega2
            * sin_delta
        - total_mass
            * parameters.g
            * std::sin(state.theta1);

    const float rhs2 =
        parameters.l1
            * state.omega1
            * state.omega1
            * sin_delta
        - parameters.g
            * std::sin(state.theta2);

    // For valid physical parameters:
    //
    // determinant = l1 * l2 * (m1 + m2 * sin(delta)^2)
    //
    // It is therefore strictly positive and does not require singularity
    // handling inside this low-level function.
    const float determinant = a * d - b * c;

    const float alpha1 = (rhs1 * d - b * rhs2) / determinant;
    const float alpha2 = (a * rhs2 - rhs1 * c) / determinant;

    return {
        state.omega1,
        state.omega2,
        alpha1,
        alpha2
    };
}

}  // namespace dpcuda
