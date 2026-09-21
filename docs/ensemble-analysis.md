# CPU Ensembles and Analysis

## Scope

This document defines the Phase 2 contracts for CPU simulation of multiple
independent double pendulums and for analyses derived from those simulations.

The initial implementation uses single-precision values and the same physical
model, parameters, timestep, and RK4 integrator as the single-system CPU
simulator.

## Ensemble input

An ensemble contains `N` initial `State` values. Every system may have different
initial angles and angular velocities.

All systems in one call share:

- one `Parameters` value;
- one timestep `dt`;
- one number of integration intervals `steps`.

Per-system physical parameters are outside the scope of Phase 2.

An empty ensemble is valid and produces empty per-system results without
evaluating the equations of motion.

## Full-trajectory mode

Full-trajectory mode stores every state for every system. With:

```text
N = number of systems
S = steps + 1
```

the result contains:

- `S` shared time samples;
- `N * S` states;
- the `N` initial states at sample zero.

The logical state shape is:

```text
(samples, systems)
```

The contiguous layout is time-major:

```text
index(sample, system) = sample * N + system
```

Thus all systems belonging to one time sample are contiguous. Public accessors
must expose the logical indices so callers do not need to reproduce the
flattening formula.

Approximate storage is:

```text
S * sizeof(float) + N * S * sizeof(State)
```

excluding container overhead. Size multiplication and allocation limits must be
checked before allocating storage.

For `N = 1`, the trajectory must be numerically equivalent to the existing
single-system simulator.

## Reduced analysis mode

Reduced analyses do not retain complete trajectories. They integrate each
system only as long as required and return one result per initial state.

Time-to-first-flip returns a signed 64-bit step index for every system:

```text
0 ... steps  = first detected flip sample
-1           = no flip detected
```

The result shape is `(N,)`.

A system may stop being integrated after its first flip. This early exit must
not change the returned step relative to applying the reference detector to a
complete trajectory.

Full trajectories and reduced analysis results use distinct result types.
Initial states, final states, trajectories, and analysis values are not stored
in one catch-all object.

## Nearby trajectories

Nearby trajectories are represented as an ordinary ensemble. A helper may
construct perturbed initial states, but simulation must use the same ensemble
engine as every other collection of initial conditions.

The perturbation and the distance scale `tau` must be explicit. Nearby-
trajectory support must not duplicate the equations of motion or RK4
integration.

## Initial-condition grids

A two-dimensional angle grid is the Cartesian product of explicit `theta1` and
`theta2` coordinate arrays. Initial angular velocities and physical parameters
are shared unless documented otherwise.

For:

```text
i = theta1 coordinate index
j = theta2 coordinate index
```

the flattened row-major index is:

```text
index(i, j) = i * theta2_count + j
```

The reduced result can therefore be reshaped to:

```text
(theta1_count, theta2_count)
```

without changing the order of its values.

## Validation and errors

The ensemble functions use the same validation rules as the single-system
simulator.

They reject:

- non-finite state components;
- invalid physical parameters;
- a non-finite or non-positive timestep;
- an invalid distance scale;
- unrepresentable result sizes.

If a future internal representation uses separate arrays for state components,
all arrays must have identical sizes. A mismatch is an error and must not cause
silent truncation or resizing.

Invalid arguments produce `std::invalid_argument`. Unrepresentable result sizes
produce `std::length_error`. Allocation failure is reported as
`std::bad_alloc`.

## Performance boundaries

Full-trajectory memory grows as `O(N * steps)`. Reduced time-to-flip memory
grows as `O(N)`.

Correctness tests and performance benchmarks remain separate. The initial CPU
benchmarks will vary:

- number of systems;
- number of integration steps;
- grid resolution;
- full versus reduced result storage.
