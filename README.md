# phosphorus - A simple physics engine

Just a simple physics simulation engine with visualization using ffmpeg.

It is also part of the project assignment for the course "Math Software" at ZJU in 2025.

## Features

- [x] Basic model
- [x] Verlet algorithm
- [x] visualization using OpenCV
- [ ] visualization using GNUplot
- [ ] GUI

## Build

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

## Project Structure

- `src/` - Source code
- `include/` - Header files
- `test/` - Unit tests using `gtest`
- `example/` - Example usage and demo
