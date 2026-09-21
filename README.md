# dp-cuda

[![CPU CI](https://github.com/ppvGH/dp-cuda/actions/workflows/cpu-ci.yml/badge.svg)](https://github.com/ppvGH/dp-cuda/actions/workflows/cpu-ci.yml)
[![codecov](https://codecov.io/gh/ppvGH/dp-cuda/graph/badge.svg)](https://codecov.io/gh/ppvGH/dp-cuda)

`dp-cuda` is a double-pendulum simulator designed to support CPU and CUDA
execution, single- and double-precision calculations, and large ensembles of
initial conditions.

The project aims to provide a documented and reproducible scientific tool with
a shared physical model across its execution backends and user interfaces.

## Project status

The C++ core currently provides the single-precision equations of motion,
mechanical-energy evaluation, fixed-step RK4 integration, and complete CPU
trajectories for one double pendulum. The command-line executable remains a
minimal project smoke check.

## Documentation

- [Scientific model](docs/scientific-model.md)
- [Numerical methods](docs/numerical-methods.md)
- [C++ API](docs/cpp-api.md)

## Requirements

- CMake 3.20 or later
- A C++17-compatible compiler

CUDA and Python are not required by the current project structure.

## Build

From the directory containing this README, configure and build the project with:

```console
cmake -S . -B build
cmake --build build --config Release
```

## Run

With a multi-configuration generator, such as Visual Studio on Windows:

```console
.\build\Release\dp-cuda.exe
```

With a single-configuration generator:

```console
./build/dp-cuda
```

The current executable prints a short confirmation message and does not yet run
a physical simulation.

## Test

Run the test suite from the directory containing this README:

```console
ctest --test-dir build -C Release --output-on-failure
```

The suite covers the executable smoke check, equations of motion, mechanical
energy, RK4 integration, simulator behavior, input validation, and energy
conservation on a documented reference trajectory.

## Current limitations

- Only single-pendulum CPU trajectories are currently implemented.
- Only single-precision calculations are currently available.
- CUDA execution is not available yet.
- Python bindings and user interfaces are not available yet.
