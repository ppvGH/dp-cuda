#include "dpcuda/energy.hpp"
#include "dpcuda/simulator.hpp"
#include "test_utils.hpp"

#include <algorithm>
#include <cstddef>
#include <cmath>
#include <stdexcept>
#include <string>

namespace {

constexpr float pi = 3.14159265358979323846F;

using test_utils::require_near;

void require_total_is_sum(const dpcuda::Energy& energy) {
    require_near(
        energy.total,
        energy.kinetic + energy.potential,
        "total energy");
}

void stationary_equilibrium_has_zero_energy() {
    const dpcuda::Energy energy = dpcuda::compute_energy({}, {});

    require_near(energy.kinetic, 0.0F, "kinetic energy");
    require_near(energy.potential, 0.0F, "potential energy");
    require_near(energy.total, 0.0F, "total energy");
}

void stationary_raised_mass_has_only_potential_energy() {
    const dpcuda::State state{pi, 0.0F, 0.0F, 0.0F};
    const dpcuda::Parameters parameters{2.0F, 3.0F, 4.0F, 5.0F, 6.0F};

    const dpcuda::Energy energy =
        dpcuda::compute_energy(state, parameters);

    // Only the first arm is inverted, so both masses rise by 2 * l1.
    constexpr float expected_potential = 240.0F;
    require_near(energy.kinetic, 0.0F, "kinetic energy");
    require_near(
        energy.potential,
        expected_potential,
        "potential energy");
    require_total_is_sum(energy);
}

void perpendicular_rods_remove_kinetic_cross_term() {
    const dpcuda::State state{0.0F, -0.5F * pi, 2.0F, -3.0F};
    const dpcuda::Parameters parameters{2.0F, 3.0F, 4.0F, 5.0F, 0.0F};

    const dpcuda::Energy energy =
        dpcuda::compute_energy(state, parameters);

    // cos(theta1 - theta2) is zero, leaving the two diagonal terms.
    constexpr float expected_kinetic = 497.5F;
    require_near(energy.kinetic, expected_kinetic, "kinetic energy");
    require_near(energy.potential, 0.0F, "potential energy");
    require_total_is_sum(energy);
}

void aligned_rods_include_kinetic_cross_term() {
    const dpcuda::State state{0.0F, 0.0F, 2.0F, -3.0F};
    const dpcuda::Parameters parameters{2.0F, 3.0F, 4.0F, 5.0F, 0.0F};

    const dpcuda::Energy energy =
        dpcuda::compute_energy(state, parameters);

    // The diagonal terms total 497.5 J and the cross term is -360 J.
    constexpr float expected_kinetic = 137.5F;
    require_near(energy.kinetic, expected_kinetic, "kinetic energy");
    require_near(energy.potential, 0.0F, "potential energy");
    require_total_is_sum(energy);
}

void zero_gravity_has_zero_potential_energy() {
    const dpcuda::State state{0.7F, -1.1F, 0.0F, 0.0F};
    dpcuda::Parameters parameters{};
    parameters.g = 0.0F;

    const dpcuda::Energy energy =
        dpcuda::compute_energy(state, parameters);

    require_near(energy.kinetic, 0.0F, "kinetic energy");
    require_near(energy.potential, 0.0F, "potential energy");
    require_total_is_sum(energy);
}

void rk4_trajectory_conserves_energy_within_studied_tolerance() {
    constexpr float dt = 0.01F;
    constexpr std::size_t steps = 1000;
    constexpr float maximum_relative_drift = 1.0e-5F;
    const dpcuda::State initial{0.7F, -0.3F, 0.4F, -0.2F};
    const dpcuda::Parameters parameters{};
    const dpcuda::SimulationResult trajectory =
        dpcuda::simulate(initial, parameters, dt, steps);
    const float initial_energy =
        dpcuda::compute_energy(initial, parameters).total;
    float maximum_drift = 0.0F;

    for (const dpcuda::State& state : trajectory.states) {
        const float energy =
            dpcuda::compute_energy(state, parameters).total;
        maximum_drift = std::max(
            maximum_drift,
            std::fabs(energy - initial_energy));
    }

    // The convergence study in numerical-methods.md found a maximum relative
    // drift of 1.57e-6 for this float trajectory. The 1e-5 threshold retains
    // margin for compiler variation while still rejecting the dt = 0.02 case.
    const float relative_drift = maximum_drift / std::fabs(initial_energy);
    if (relative_drift > maximum_relative_drift) {
        throw std::runtime_error(
            "maximum relative energy drift exceeds tolerance: "
            + std::to_string(relative_drift));
    }
}

}  // namespace

int main() {
    test_utils::Runner runner;

    runner.run(
        "stationary_equilibrium_has_zero_energy",
        stationary_equilibrium_has_zero_energy);
    runner.run(
        "stationary_raised_mass_has_only_potential_energy",
        stationary_raised_mass_has_only_potential_energy);
    runner.run(
        "perpendicular_rods_remove_kinetic_cross_term",
        perpendicular_rods_remove_kinetic_cross_term);
    runner.run(
        "aligned_rods_include_kinetic_cross_term",
        aligned_rods_include_kinetic_cross_term);
    runner.run(
        "zero_gravity_has_zero_potential_energy",
        zero_gravity_has_zero_potential_energy);
    runner.run(
        "rk4_trajectory_conserves_energy_within_studied_tolerance",
        rk4_trajectory_conserves_energy_within_studied_tolerance);

    return runner.exit_code();
}
