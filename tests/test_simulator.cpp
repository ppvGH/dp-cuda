#include "dpcuda/simulator.hpp"
#include "test_utils.hpp"

#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>

namespace {

using test_utils::require_near;
using test_utils::require_state_near;

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

template <typename Operation>
void require_invalid_argument(Operation operation) {
    try {
        operation();
    } catch (const std::invalid_argument&) {
        return;
    }

    throw std::runtime_error("expected std::invalid_argument");
}

void zero_steps_returns_only_initial_sample() {
    const dpcuda::State initial{0.3F, -0.2F, 0.1F, -0.4F};
    const dpcuda::Parameters parameters{};

    const dpcuda::SimulationResult result =
        dpcuda::simulate(initial, parameters, 0.01F, 0);

    require(result.times.size() == 1, "expected one time sample");
    require(result.states.size() == 1, "expected one state sample");
    require_near(result.times.front(), 0.0F, "initial time");
    require_state_near(result.states.front(), initial);
}

void sample_count_and_times_follow_contract() {
    constexpr float dt = 0.25F;
    constexpr std::size_t steps = 4;

    const dpcuda::SimulationResult result =
        dpcuda::simulate({}, {}, dt, steps);

    require(result.times.size() == steps + 1, "unexpected time count");
    require(result.states.size() == steps + 1, "unexpected state count");

    for (std::size_t sample = 0; sample <= steps; ++sample) {
        require_near(
            result.times[sample],
            static_cast<float>(sample) * dt,
            "sample time");
    }
}

void equilibrium_remains_stationary() {
    constexpr std::size_t steps = 100;
    const dpcuda::State equilibrium{};

    const dpcuda::SimulationResult result =
        dpcuda::simulate(equilibrium, {}, 0.01F, steps);

    for (const dpcuda::State& state : result.states) {
        require_state_near(state, equilibrium);
    }
}

void ordinary_trajectory_remains_finite() {
    const dpcuda::State initial{0.7F, -0.3F, 0.4F, -0.2F};

    const dpcuda::SimulationResult result =
        dpcuda::simulate(initial, {}, 0.001F, 1000);

    for (const dpcuda::State& state : result.states) {
        require(std::isfinite(state.theta1), "theta1 is not finite");
        require(std::isfinite(state.theta2), "theta2 is not finite");
        require(std::isfinite(state.omega1), "omega1 is not finite");
        require(std::isfinite(state.omega2), "omega2 is not finite");
    }
}

void identical_inputs_produce_identical_trajectories() {
    const dpcuda::State initial{0.7F, -0.3F, 0.4F, -0.2F};

    // Exact comparisons intentionally test bit-for-bit reproducibility for
    // repeated executions with the same inputs, not numerical proximity.
    const dpcuda::SimulationResult first =
        dpcuda::simulate(initial, {}, 0.01F, 20);
    const dpcuda::SimulationResult second =
        dpcuda::simulate(initial, {}, 0.01F, 20);

    require(first.times == second.times, "time samples differ");
    require(first.states.size() == second.states.size(), "state counts differ");

    for (std::size_t sample = 0; sample < first.states.size(); ++sample) {
        const dpcuda::State& lhs = first.states[sample];
        const dpcuda::State& rhs = second.states[sample];

        require(
            lhs.theta1 == rhs.theta1
                && lhs.theta2 == rhs.theta2
                && lhs.omega1 == rhs.omega1
                && lhs.omega2 == rhs.omega2,
            "state samples differ");
    }
}

void non_finite_initial_state_is_rejected() {
    dpcuda::State state{};
    state.theta1 = std::numeric_limits<float>::quiet_NaN();

    require_invalid_argument([&state] {
        static_cast<void>(dpcuda::simulate(state, {}, 0.01F, 1));
    });
}

void non_positive_mass_is_rejected() {
    dpcuda::Parameters parameters{};
    parameters.m1 = 0.0F;

    require_invalid_argument([&parameters] {
        static_cast<void>(dpcuda::simulate({}, parameters, 0.01F, 1));
    });
}

void non_positive_length_is_rejected() {
    dpcuda::Parameters parameters{};
    parameters.l2 = -1.0F;

    require_invalid_argument([&parameters] {
        static_cast<void>(dpcuda::simulate({}, parameters, 0.01F, 1));
    });
}

void negative_gravity_is_rejected() {
    dpcuda::Parameters parameters{};
    parameters.g = -1.0F;

    require_invalid_argument([&parameters] {
        static_cast<void>(dpcuda::simulate({}, parameters, 0.01F, 1));
    });
}

void non_positive_timestep_is_rejected() {
    require_invalid_argument([] {
        static_cast<void>(dpcuda::simulate({}, {}, 0.0F, 1));
    });
}

void non_finite_timestep_is_rejected() {
    require_invalid_argument([] {
        static_cast<void>(dpcuda::simulate(
            {},
            {},
            std::numeric_limits<float>::infinity(),
            1));
    });
}

void unrepresentable_sample_count_is_rejected() {
    try {
        static_cast<void>(dpcuda::simulate(
            {},
            {},
            0.01F,
            std::numeric_limits<std::size_t>::max()));
    } catch (const std::length_error&) {
        return;
    }

    throw std::runtime_error("expected std::length_error");
}

}  // namespace

int main() {
    test_utils::Runner runner;

    runner.run(
        "zero_steps_returns_only_initial_sample",
        zero_steps_returns_only_initial_sample);
    runner.run(
        "sample_count_and_times_follow_contract",
        sample_count_and_times_follow_contract);
    runner.run("equilibrium_remains_stationary", equilibrium_remains_stationary);
    runner.run(
        "ordinary_trajectory_remains_finite",
        ordinary_trajectory_remains_finite);
    runner.run(
        "identical_inputs_produce_identical_trajectories",
        identical_inputs_produce_identical_trajectories);
    runner.run(
        "non_finite_initial_state_is_rejected",
        non_finite_initial_state_is_rejected);
    runner.run("non_positive_mass_is_rejected", non_positive_mass_is_rejected);
    runner.run(
        "non_positive_length_is_rejected",
        non_positive_length_is_rejected);
    runner.run("negative_gravity_is_rejected", negative_gravity_is_rejected);
    runner.run(
        "non_positive_timestep_is_rejected",
        non_positive_timestep_is_rejected);
    runner.run(
        "non_finite_timestep_is_rejected",
        non_finite_timestep_is_rejected);
    runner.run(
        "unrepresentable_sample_count_is_rejected",
        unrepresentable_sample_count_is_rejected);

    return runner.exit_code();
}
