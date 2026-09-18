#pragma once

namespace dpcuda {

    // Angles in rad; angular velocities in rad/s.
    struct State {
        float theta1{};
        float theta2{};
        float omega1{};
        float omega2{};
    };

    // Masses in kg, lengths in m, gravity in m/s^2.
    struct Parameters {
        float m1{ 1.0F };
        float m2{ 1.0F };
        float l1{ 1.0F };
        float l2{ 1.0F };
        float g{ 9.80665F };
    };

}  // namespace dpcuda