# phosphorus - A simple physics engine

**WARNING: This library's architecture suffers from significant design limitations and necessitates a comprehensive
overhaul.**

Just a simple physics simulation engine with visualization using ffmpeg.

It is also part of the project assignment for the course "Math Software" at ZJU in 2025.

## Features

- [x] Basic model
- [x] Verlet algorithm
- [x] visualization using OpenCV
- [ ] GUI

## Build

### Dependencies

Some C++20 features like `format` and `concept` is used in this project, so make sure you have a C++20 compatible
compiler. The project has been tested with GCC 15 and MSVC 19.43.34809. It should work with GCC 13 and later versions as
well.

We use the following libraries:

- [OpenCV](https://opencv.org/) - For image processing and visualization
- [Boost](https://www.boost.org/) - For various utilities
- [GTest](https://github.com/google/googletest) - For unit testing (Only if `PHOSPHORUS_BUILD_TESTS` is enabled)
- [range-v3](https://github.com/ericniebler/range-v3) - For range-based algorithms. It is also included in the project.

Besides, `gnuplot` is required for visualization, but is not as a library dependency. Please make sure that `gnuplot` is
in your `PATH` environment variable.

### Building - CMake

We use CMake to build the project. You can build it using the following commands:

```bash
mkdir build
cd build
cmake .. -G Ninja
cmake --build . -j${nproc}
```

Also, there are some options listed below:

- `PHOSPHORUS_BUILD_TESTS` - Build the tests (default: `ON`, requires `gtest`)
- `PHOSPHORUS_BUILD_EXAMPLE` - Build the example (default: `ON`)

Besides, we provide a `Makefile` to automatically call CMake and build the project.

## Project Structure

- `src/` - Source code
- `include/` - Header files
- `test/` - Unit tests using `gtest`
- `example/` - Example usage and demo
    - `Task1.cpp` - Example for Task 1: spring simulation
    - `Task2-1.cpp` - Example for Task 2-1: Sun and Earth simulation
    - `Task2-2.cpp` - Example for Task 2-2: Sun and Earth simulation with initial velocity
    - `Task3.cpp` - Example for Task 3: three-body problem simulation
