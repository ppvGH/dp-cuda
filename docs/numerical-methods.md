# Numerical Methods

## Scope

This document defines the time-discretization, integration, sampling, precision,
and numerical-accuracy conventions used by `dp-cuda`.

The classical fourth-order Runge-Kutta method with a fixed timestep is the
reference integration method. Additional numerical methods may be documented
here together with their assumptions, accuracy, and limitations.

The physical equations and coordinate conventions are documented in
[Scientific Model](scientific-model.md).

## Time discretization

The simulator uses a fixed timestep named `dt`, expressed in seconds.

The parameter `steps` denotes the number of integration intervals. 
Given an initial state at time zero, a simulation returns:

- `steps + 1` state samples;
- `steps + 1` time samples;
- the initial state as sample `0`;
- the state after the last integration step as sample `steps`.

The time associated with sample `i` is:

```text
time[i] = i * dt, for i = 0, ..., steps
```

The total simulated duration is:

```text
duration = steps * dt
```

When `steps` is zero, the result contains only the initial state at `t = 0`.

## Runge-Kutta method

For an autonomous system:

```text
dy / dt = f(y)
```

one classical RK4 step from `y[n]` to `y[n + 1]` is computed as:

```text
k1 = f(y[n])
k2 = f(y[n] + 0.5 * dt * k1)
k3 = f(y[n] + 0.5 * dt * k2)
k4 = f(y[n] + dt * k3)

y[n + 1] = y[n] + (dt / 6) * (k1 + 2*k2 + 2*k3 + k4)
```

The method has fourth-order global accuracy for sufficiently smooth problems,
but it is not symplectic and does not conserve energy exactly.

## Numerical precision

Finite-precision arithmetic introduces rounding at every stage of the
calculation. All calculations belonging to one simulation use a consistent
floating-point precision, including state values, physical parameters,
intermediate integration stages, and trajectory values.

Different floating-point precisions can accumulate different rounding errors.
For a chaotic trajectory, these initially small differences may grow until the
computed states no longer agree point by point, even when both calculations
follow the same physical model and numerical method.

## Accuracy and convergence

For sufficiently smooth problems, classical RK4 has a global discretization
error proportional to the fourth power of the timestep. In its asymptotic
regime, reducing `dt` should therefore make the numerical solution converge
towards the continuous solution.

Classical RK4 does not conserve mechanical energy exactly. Energy drift over a
finite duration is one useful indicator of numerical accuracy, but it does not
provide a complete error estimate on its own.

A convergence study compares solutions obtained over the same duration with
successively smaller timesteps. Timestep selection must consider convergence,
energy drift, execution time, floating-point precision, and the range of
physical configurations the software is intended to handle.

### Float CPU energy study

The energy-conservation test uses the default physical parameters and the
initial state:

```text
(theta1, theta2, omega1, omega2) = (0.7, -0.3, 0.4, -0.2)
```

The same 10-second trajectory was simulated in `float` with successively
halved timesteps. The final-state error is the largest absolute component
difference relative to the `dt = 0.00125 s` result.

| `dt` (s) | Maximum absolute energy drift (J) | Maximum relative energy drift | Final-state error |
| ---: | ---: | ---: | ---: |
| 0.08 | 1.699e-1 | 3.275e-2 | 4.297e-1 |
| 0.04 | 7.071e-3 | 1.363e-3 | 1.491e-2 |
| 0.02 | 2.322e-4 | 4.477e-5 | 4.325e-4 |
| 0.01 | 8.106e-6 | 1.563e-6 | 1.290e-5 |
| 0.005 | 1.192e-5 | 2.298e-6 | 3.004e-5 |
| 0.0025 | 1.669e-5 | 3.218e-6 | 4.351e-5 |
| 0.00125 | 2.050e-5 | 3.953e-6 | reference |

From `dt = 0.08 s` through `dt = 0.01 s`, both indicators decrease rapidly
as the timestep is halved. Below `0.01 s`, single-precision rounding dominates
this particular experiment and the measured errors no longer decrease
monotonically.

The automated conservation test therefore uses `dt = 0.01 s`, 1000 steps, and
a maximum relative energy-drift tolerance of `1.0e-5`. This is about 6.4 times
the measured drift at that timestep, allowing for compiler variation, while
remaining below the `4.477e-5` drift measured at `dt = 0.02 s`.

This controlled study justifies the timestep and tolerance of the test; it does
not establish a universal default or supported timestep range. More energetic
or strongly chaotic trajectories may require a smaller timestep. A user-facing
default and supported range remain to be selected from a broader validation
set.

## Discrete first-flip detection

The initial CPU implementation detects flips at stored integration samples. Let
`state[i]` be the state at:

```text
time[i] = i * dt
```

The first-flip step is the smallest index `i` for which:

```text
abs(theta1[i]) >= pi or abs(theta2[i]) >= pi
```

Sample zero is included. Therefore an initial state that already satisfies the
condition has first-flip step `0`.

The Phase 2 baseline does not interpolate the event inside an integration
interval. Its reported time is quantized to:

```text
first_flip_time = first_flip_step * dt
```

If no flip is detected through sample `steps`, the step value is `-1`. This
fixed signed sentinel is intended to remain consistent across C++, Python, CPU,
and CUDA implementations.

A timestep that is too large may provide a poor approximation of the continuous
event time. Event-time accuracy must therefore be studied separately from the
correctness of the detector.

## Distance between states

Nearby-trajectory analysis uses the unwrapped angular differences:

```text
delta_theta1 = theta1_b - theta1_a
delta_theta2 = theta2_b - theta2_a
delta_omega1 = omega1_b - omega1_a
delta_omega2 = omega2_b - omega2_a
```

The angles are deliberately not reduced modulo `2*pi`, because complete
rotations are part of the simulated state.

To avoid directly combining angular positions and angular velocities with
different units, the distance uses an explicit positive reference time `tau`:

```text
distance_tau =
    sqrt(delta_theta1^2
         + delta_theta2^2
         + (tau * delta_omega1)^2
         + (tau * delta_omega2)^2)
```

`tau` is expressed in seconds and must be finite and greater than zero. It is a
property of the analysis, not of the physical model, and must be reported with
the result.

## Limitations

- The reference integrator uses a fixed timestep and provides no adaptive error
  estimator.
- Classical RK4 does not preserve the Hamiltonian structure exactly.
- A timestep suitable for one initial condition may be too large for another.
- Small energy drift over a finite interval does not guarantee accuracy for
  every trajectory or duration.
- Chaotic divergence limits meaningful point-by-point comparisons over long
  times.

## Timestep guidance

Smaller timesteps generally reduce integration error but increase execution time
and, when full trajectories are retained, memory use. No single timestep is
appropriate for every initial condition and duration. The current C++ API
therefore requires callers to select an explicit timestep. A future
user-facing default and supported range will require validation over a broader
set of physical configurations.
