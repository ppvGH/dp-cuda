#include "dpcuda/dynamics.hpp"
#include "dpcuda/rk4.hpp"
#include "test_utils.hpp"

#include <cmath>

namespace {

using test_utils::require_near;
using test_utils::require_state_near;

void zero_derivative_preserves_state() {
    const dpcuda::State initial{0.3F, -0.7F, 1.2F, -0.4F};
    const auto zero_derivative = [](const dpcuda::State&) {
        return dpcuda::State{};
    };

    const dpcuda::State result =
        dpcuda::rk4_step(initial, 0.1F, zero_derivative);

    require_state_near(result, initial);
}

void constant_derivative_is_integrated_exactly() {
    const dpcuda::State initial{1.0F, -2.0F, 0.5F, 4.0F};
    const dpcuda::State rate{2.0F, -1.0F, 3.0F, 0.25F};
    const auto constant_derivative = [&rate](const dpcuda::State&) {
        return rate;
    };

    constexpr float dt = 0.125F;
    const dpcuda::State expected{
        initial.theta1 + dt * rate.theta1,
        initial.theta2 + dt * rate.theta2,
        initial.omega1 + dt * rate.omega1,
        initial.omega2 + dt * rate.omega2
    };

    const dpcuda::State result =
        dpcuda::rk4_step(initial, dt, constant_derivative);

    require_state_near(result, expected);
}

void exponential_equation_matches_analytical_solution() {
    dpcuda::State state{1.0F, 0.0F, 0.0F, 0.0F};
    // The scalar equation y' = y with y(0) = 1 has the exact solution
    // y(t) = exp(t), providing a controlled accuracy check at t = 1.
    const auto exponential_derivative = [](const dpcuda::State& current) {
        return dpcuda::State{current.theta1, 0.0F, 0.0F, 0.0F};
    };

    constexpr float dt = 0.1F;
    constexpr int steps = 10;

    for (int step = 0; step < steps; ++step) {
        state = dpcuda::rk4_step(state, dt, exponential_derivative);
    }

    require_near(state.theta1, std::exp(1.0F), "solution at t = 1");
    require_near(state.theta2, 0.0F, "theta2");
    require_near(state.omega1, 0.0F, "omega1");
    require_near(state.omega2, 0.0F, "omega2");
}

void pendulum_equilibrium_is_preserved() {
    const dpcuda::State equilibrium{};
    const dpcuda::Parameters parameters{};
    const auto dynamics = [&parameters](const dpcuda::State& state) {
        return dpcuda::compute_state_derivative(state, parameters);
    };

    const dpcuda::State result =
        dpcuda::rk4_step(equilibrium, 0.1F, dynamics);

    require_state_near(result, equilibrium);
}

}  // namespace

int main() {
    test_utils::Runner runner;

    runner.run("zero_derivative_preserves_state", zero_derivative_preserves_state);
    runner.run(
        "constant_derivative_is_integrated_exactly",
        constant_derivative_is_integrated_exactly);
    runner.run(
        "exponential_equation_matches_analytical_solution",
        exponential_equation_matches_analytical_solution);
    runner.run(
        "pendulum_equilibrium_is_preserved",
        pendulum_equilibrium_is_preserved);

    return runner.exit_code();
}
