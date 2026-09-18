# C++ API

## Scope

This document describes the public C++ interface of `dp-cuda`. The current API
simulates one double pendulum on the CPU using single-precision floating-point
values and fixed-step RK4 integration.

The physical conventions are defined in the
[scientific model](scientific-model.md). Time discretization and RK4 are
described in [numerical methods](numerical-methods.md).

## State

Header:

```cpp
#include "dpcuda/types.hpp"
```

`dpcuda::State` stores the two absolute angles and their angular velocities:

```cpp
struct State {
    float theta1;
    float theta2;
    float omega1;
    float omega2;
};
```

Angles are expressed in radians and angular velocities in radians per second.
The default-constructed state is the stationary hanging equilibrium.

## Parameters

`dpcuda::Parameters` stores the physical properties of one double pendulum:

| Field | Meaning | Default | Requirement |
| --- | --- | --- | --- |
| `m1` | First mass | `1.0 kg` | Finite and greater than zero |
| `m2` | Second mass | `1.0 kg` | Finite and greater than zero |
| `l1` | First rod length | `1.0 m` | Finite and greater than zero |
| `l2` | Second rod length | `1.0 m` | Finite and greater than zero |
| `g` | Gravitational acceleration magnitude | `9.80665 m/s^2` | Finite and non-negative |

## SimulationResult

Header:

```cpp
#include "dpcuda/simulation_result.hpp"
```

A trajectory is returned as:

```cpp
struct SimulationResult {
    std::vector<float> times;
    std::vector<State> states;
};
```

The two vectors always have the same size. `times[i]` is the time in seconds
associated with `states[i]`. Sample zero contains the supplied initial state at
`t = 0`.

For `steps` integration intervals, both vectors contain `steps + 1` elements and
the last sample corresponds to `steps * dt`.

## simulate

Header:

```cpp
#include "dpcuda/simulator.hpp"
```

Signature:

```cpp
dpcuda::SimulationResult dpcuda::simulate(
    const dpcuda::State& initial_state,
    const dpcuda::Parameters& parameters,
    float dt,
    std::size_t steps
);
```

Parameters:

- `initial_state`: state stored as sample zero;
- `parameters`: physical parameters used throughout the trajectory;
- `dt`: fixed timestep in seconds, finite and greater than zero;
- `steps`: number of RK4 integration intervals.

The C++ core currently requires an explicit `dt`. A documented user-facing
default and supported range will be selected after convergence and energy-drift
studies.

Example:

```cpp
#include "dpcuda/simulator.hpp"

const dpcuda::State initial{0.7F, -0.3F, 0.0F, 0.0F};
const dpcuda::Parameters parameters{};

const dpcuda::SimulationResult result =
    dpcuda::simulate(initial, parameters, 0.001F, 1000);
```

## Errors

`simulate` throws `std::invalid_argument` when:

- a component of the initial state is not finite;
- a mass or length is not finite or is not greater than zero;
- gravity is not finite or is negative;
- `dt` is not finite or is not greater than zero.

It throws `std::length_error` when `steps + 1` is not representable or when the
requested number of samples exceeds the maximum supported vector size. Memory
allocation failure is reported by the standard library as `std::bad_alloc`.

## Memory usage

The returned trajectory stores every time and state sample. Its memory usage
therefore grows linearly with `steps + 1` and is approximately:

```text
(steps + 1) * (sizeof(float) + sizeof(State))
```

excluding the small fixed overhead of the two vectors. Analyses that do not
require a complete trajectory will use reduced result types rather than this
interface.

