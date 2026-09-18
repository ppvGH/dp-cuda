# dp-cuda

`dp-cuda` is a double-pendulum simulator designed to support CPU and CUDA
execution, single- and double-precision calculations, and large ensembles of
initial conditions.

The project aims to provide a documented and reproducible scientific tool with
a shared physical model across its execution backends and user interfaces.

## Project status

The repository currently contains the initial C++ project structure, a minimal
executable, and a CTest smoke test. The physical model and numerical conventions
have been defined, while the CPU simulator is under development.

## Documentation

- [Scientific model](docs/scientific-model.md)
- [Numerical methods](docs/numerical-methods.md)

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

The current suite contains a smoke test that verifies that the executable starts
and produces the expected confirmation message.

## Current limitations

- The physical simulation is not implemented yet.
- Only the initial CPU-oriented C++ structure is available.
- CUDA execution is not available yet.
- Python bindings and user interfaces are not available yet.

