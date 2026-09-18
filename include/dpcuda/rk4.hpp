#pragma once

#include "dpcuda/types.hpp"

namespace dpcuda {

namespace internal {

inline State add_scaled(
    const State& state,
    const State& derivative,
    float scale
) {
    return {
        state.theta1 + scale * derivative.theta1,
        state.theta2 + scale * derivative.theta2,
        state.omega1 + scale * derivative.omega1,
        state.omega2 + scale * derivative.omega2
    };
}

inline State rk4_update(
    const State& state,
    const State& k1,
    const State& k2,
    const State& k3,
    const State& k4,
    float dt
) {
    const float scale = dt / 6.0F;

    return {
        state.theta1
            + scale * (k1.theta1 + 2.0F * k2.theta1
                       + 2.0F * k3.theta1 + k4.theta1),
        state.theta2
            + scale * (k1.theta2 + 2.0F * k2.theta2
                       + 2.0F * k3.theta2 + k4.theta2),
        state.omega1
            + scale * (k1.omega1 + 2.0F * k2.omega1
                       + 2.0F * k3.omega1 + k4.omega1),
        state.omega2
            + scale * (k1.omega2 + 2.0F * k2.omega2
                       + 2.0F * k3.omega2 + k4.omega2)
    };
}

}  // namespace internal

// Advances one state by one fixed RK4 timestep.
//
// The derivative callable maps a State to its time derivative. This low-level
// operation assumes a finite, positive timestep and valid input data.
// Templating the derivative keeps RK4 independent of the physical model and
// avoids the runtime indirection of a type-erased callable such as std::function.
template <typename Derivative>
State rk4_step(
    const State& state,
    float dt,
    const Derivative& compute_derivative
) {
    const State k1 = compute_derivative(state);

    const State k2 = compute_derivative(
        internal::add_scaled(state, k1, 0.5F * dt)
    );

    const State k3 = compute_derivative(
        internal::add_scaled(state, k2, 0.5F * dt)
    );

    const State k4 = compute_derivative(
        internal::add_scaled(state, k3, dt)
    );

    return internal::rk4_update(state, k1, k2, k3, k4, dt);
}

}  // namespace dpcuda

