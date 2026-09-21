#pragma once

#include "dpcuda/types.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

namespace test_utils {

inline constexpr float default_tolerance = 1.0e-5F;

// Combines an absolute tolerance near zero with a relative tolerance for
// values of larger magnitude.
inline bool nearly_equal(
    float actual,
    float expected,
    float tolerance = default_tolerance
) {
    const float scale = std::max(std::fabs(actual), std::fabs(expected));
    return std::fabs(actual - expected)
        <= tolerance * (1.0F + scale);
}

inline void require_near(
    float actual,
    float expected,
    const std::string& quantity,
    float tolerance = default_tolerance
) {
    if (!nearly_equal(actual, expected, tolerance)) {
        throw std::runtime_error(
            quantity + ": expected " + std::to_string(expected)
            + ", got " + std::to_string(actual));
    }
}

inline void require_state_near(
    const dpcuda::State& actual,
    const dpcuda::State& expected,
    float tolerance = default_tolerance
) {
    require_near(actual.theta1, expected.theta1, "theta1", tolerance);
    require_near(actual.theta2, expected.theta2, "theta2", tolerance);
    require_near(actual.omega1, expected.omega1, "omega1", tolerance);
    require_near(actual.omega2, expected.omega2, "omega2", tolerance);
}

class Runner {
public:
    template <typename Test>
    void run(const char* name, const Test& test) {
        try {
            test();
        } catch (const std::exception& error) {
            ++failures_;
            std::cerr << "[FAIL] " << name << ": " << error.what() << '\n';
        }
    }

    int exit_code() const {
        return failures_ == 0 ? 0 : 1;
    }

private:
    int failures_{};
};

}  // namespace test_utils
