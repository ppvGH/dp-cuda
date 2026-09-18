#include "dpcuda/dynamics.hpp"
#include "dpcuda/rk4.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

constexpr float tolerance = 1.0e-5F;

// The constant term provides an absolute tolerance near zero, while scale
// provides a relative tolerance for values with larger magnitude.
bool nearly_equal(float actual, float expected) {
    const float scale = std::max(std::fabs(actual), std::fabs(expected));
    return std::fabs(actual - expected) <= tolerance * (1.0F + scale);
}

void require_near(float actual, float expected, const std::string& quantity) {
    if (!nearly_equal(actual, expected)) {
        throw std::runtime_error(
            quantity + ": expected " + std::to_string(expected)
            + ", got " + std::to_string(actual));
    }
}

void require_state_near(
    const dpcuda::State& actual,
    const dpcuda::State& expected
) {
    require_near(actual.theta1, expected.theta1, "theta1");
    require_near(actual.theta2, expected.theta2, "theta2");
    require_near(actual.omega1, expected.omega1, "omega1");
    require_near(actual.omega2, expected.omega2, "omega2");
}

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
    int failures = 0;

    const auto run = [&failures](const char* name, auto test) {
        try {
            test();
        } catch (const std::exception& error) {
            ++failures;
            std::cerr << "[FAIL] " << name << ": " << error.what() << '\n';
        }
    };

    run("zero_derivative_preserves_state", zero_derivative_preserves_state);
    run(
        "constant_derivative_is_integrated_exactly",
        constant_derivative_is_integrated_exactly);
    run(
        "exponential_equation_matches_analytical_solution",
        exponential_equation_matches_analytical_solution);
    run("pendulum_equilibrium_is_preserved", pendulum_equilibrium_is_preserved);

    return failures == 0 ? 0 : 1;
}

