#include "dpcuda/simulator.hpp"

#include "dpcuda/dynamics.hpp"
#include "dpcuda/rk4.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

namespace dpcuda {

namespace internal {

void require_finite(float value, const char* name) {
    if (!std::isfinite(value)) {
        throw std::invalid_argument(std::string{name} + " must be finite");
    }
}

void require_positive(float value, const char* name) {
    require_finite(value, name);
    if (value <= 0.0F) {
        throw std::invalid_argument(
            std::string{name} + " must be greater than zero");
    }
}

void require_non_negative(float value, const char* name) {
    require_finite(value, name);
    if (value < 0.0F) {
        throw std::invalid_argument(
            std::string{name} + " must be non-negative");
    }
}

void validate_simulation_inputs(
    const State& state,
    const Parameters& parameters,
    float dt,
    std::size_t steps
) {
    require_finite(state.theta1, "initial_state.theta1");
    require_finite(state.theta2, "initial_state.theta2");
    require_finite(state.omega1, "initial_state.omega1");
    require_finite(state.omega2, "initial_state.omega2");

    require_positive(parameters.m1, "parameters.m1");
    require_positive(parameters.m2, "parameters.m2");
    require_positive(parameters.l1, "parameters.l1");
    require_positive(parameters.l2, "parameters.l2");
    require_non_negative(parameters.g, "parameters.g");
    require_positive(dt, "dt");

    if (steps == std::numeric_limits<std::size_t>::max()) {
        throw std::length_error("steps + 1 is not representable");
    }
}

}  // namespace internal

SimulationResult simulate(
    const State& initial_state,
    const Parameters& parameters,
    float dt,
    std::size_t steps
) {
    internal::validate_simulation_inputs(
        initial_state,
        parameters,
        dt,
        steps
    );

    const std::size_t sample_count = steps + 1;
    SimulationResult result;

    if (sample_count > result.times.max_size()
        || sample_count > result.states.max_size()) {
        throw std::length_error("requested trajectory is too large");
    }

    result.times.reserve(sample_count);
    result.states.reserve(sample_count);

    State current = initial_state;
    result.times.push_back(0.0F);
    result.states.push_back(current);

    const auto dynamics = [&parameters](const State& state) {
        return compute_state_derivative(state, parameters);
    };

    for (std::size_t step = 1; step < sample_count; ++step) {
        current = rk4_step(current, dt, dynamics);
        result.times.push_back(static_cast<float>(step) * dt);
        result.states.push_back(current);
    }

    return result;
}

}  // namespace dpcuda

