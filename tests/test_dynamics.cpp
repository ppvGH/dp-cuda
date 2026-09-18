#include "dpcuda/dynamics.hpp"

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

void require_finite(float value, const std::string& quantity) {
    if (!std::isfinite(value)) {
        throw std::runtime_error(quantity + " is not finite");
    }
}

void equilibrium_has_zero_derivative() {
    const dpcuda::State equilibrium{};
    const dpcuda::Parameters parameters{};

    const dpcuda::State result =
        dpcuda::compute_state_derivative(equilibrium, parameters);

    require_near(result.theta1, 0.0F, "dtheta1/dt");
    require_near(result.theta2, 0.0F, "dtheta2/dt");
    require_near(result.omega1, 0.0F, "domega1/dt");
    require_near(result.omega2, 0.0F, "domega2/dt");
}

void angular_derivatives_equal_angular_velocities() {
    const dpcuda::State state{0.2F, -0.4F, 1.25F, -2.5F};
    const dpcuda::Parameters parameters{};

    const dpcuda::State result =
        dpcuda::compute_state_derivative(state, parameters);

    require_near(result.theta1, state.omega1, "dtheta1/dt");
    require_near(result.theta2, state.omega2, "dtheta2/dt");
}

void ordinary_state_has_finite_derivative() {
    const dpcuda::State state{0.7F, -0.3F, 0.4F, -0.2F};
    const dpcuda::Parameters parameters{};

    const dpcuda::State result =
        dpcuda::compute_state_derivative(state, parameters);

    require_finite(result.theta1, "dtheta1/dt");
    require_finite(result.theta2, "dtheta2/dt");
    require_finite(result.omega1, "domega1/dt");
    require_finite(result.omega2, "domega2/dt");
}

void sign_reversal_reverses_derivative() {
    const dpcuda::State state{0.7F, -0.3F, 0.4F, -0.2F};
    const dpcuda::State reversed_state{
        -state.theta1,
        -state.theta2,
        -state.omega1,
        -state.omega2
    };
    const dpcuda::Parameters parameters{};

    const dpcuda::State result =
        dpcuda::compute_state_derivative(state, parameters);
    const dpcuda::State reversed_result =
        dpcuda::compute_state_derivative(reversed_state, parameters);

    require_near(reversed_result.theta1, -result.theta1, "reversed dtheta1/dt");
    require_near(reversed_result.theta2, -result.theta2, "reversed dtheta2/dt");
    require_near(reversed_result.omega1, -result.omega1, "reversed domega1/dt");
    require_near(reversed_result.omega2, -result.omega2, "reversed domega2/dt");
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

    run("equilibrium_has_zero_derivative", equilibrium_has_zero_derivative);
    run(
        "angular_derivatives_equal_angular_velocities",
        angular_derivatives_equal_angular_velocities);
    run("ordinary_state_has_finite_derivative", ordinary_state_has_finite_derivative);
    run("sign_reversal_reverses_derivative", sign_reversal_reverses_derivative);

    return failures == 0 ? 0 : 1;
}
