#pragma once

namespace dpcuda {

    // Complete state of one double pendulum.
    //
    // theta1 and theta2 are absolute angles measured counterclockwise from the
    // downward vertical. Angles are expressed in radians and are not wrapped.
    // omega1 and omega2 are the corresponding angular velocities in rad/s.
    struct State {
        float theta1{};
        float theta2{};
        float omega1{};
        float omega2{};
    };

    // Physical parameters of one double pendulum, expressed in SI units.
    //
    // Masses and lengths must be finite and greater than zero. Gravity is
    // treated as a magnitude and must be finite and non-negative.
    struct Parameters {
        float m1{ 1.0F };
        float m2{ 1.0F };
        float l1{ 1.0F };
        float l2{ 1.0F };
        float g{ 9.80665F };
    };

}  // namespace dpcuda
