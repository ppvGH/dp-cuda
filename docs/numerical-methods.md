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
successively smaller timesteps. The default timestep and the supported
user-selectable range will be chosen by considering convergence, energy drift,
execution time, and the range of physical configurations the software is
intended to handle.

TODO: Determine the default `dt` and supported range through convergence and
energy-drift studies.

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
appropriate for every initial condition and duration. The software therefore
provides a documented default while allowing users to select another value
within the supported range.
