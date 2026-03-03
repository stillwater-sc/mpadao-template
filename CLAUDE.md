# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Mixed-Precision Algorithm Development and Optimization (MPADAO) template for IoT, ML/DL/RL, SDR, and DSP applications targeting custom hardware accelerators. Built by Stillwater Supercomputing, Inc. Uses C++20 throughout.

## Build Commands

```bash
# Initialize submodules (required first time)
git submodule init && git submodule update

# Standard build
mkdir build && cd build
cmake ..
make -j$(nproc)

# Install (installs to project root: bin/, lib/, include/)
make install

# Run all tests
cd build && ctest

# Build without submodule demos/tests (faster, used in CI)
cmake -DBUILD_DEMONSTRATION=OFF -DENABLE_TESTS=OFF ..

# Build with Abseil logging support
cmake -DMPADAO_ENABLE_ABSEIL=ON ..

# Debug build (no -O3)
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Single test binary
./build/test/mpadao_tests
```

## Architecture

**Dependencies (git submodules in `ext/`):**
- `stillwater-sc/universal` — header-only number system library (posits, cfloats, integers, fixed-point, etc.)
- `stillwater-sc/mtl4` — header-only matrix/linear algebra library
- `google/googletest` — test framework
- `google/abseil` — logging and utilities (C++20 required, optional via `MPADAO_ENABLE_ABSEIL`)

**Source layout (`src/`):**
- `apps/` — example applications demonstrating mixed-precision concepts
  - `universal/` — basic Universal number system usage (cfloat ops, ULP calculations)
  - `polynomial/` — polynomial evaluation across multiple number types (integers, fixpnts, areals, cfloats, posits, logs, unums)
  - `quadratic/` — catastrophic cancellation in quadratic formula (educational)
  - `logging/` — Abseil logging integration (only built when `MPADAO_ENABLE_ABSEIL=ON`)
- `solvers/` — production-like algorithms (Gauss-Jordan matrix inversion using MTL4 + Boost multiprecision)
- `mtl4ext/` — MTL4 extension headers (matrix generators, vector utils, norms)
- `lib/version/` — static library providing `mpadao::get_semver()` version info; version synced from CMake via `configure_file`
- `tools/semver/` — CLI tool for semantic version output

**Test/Benchmark layout:**
- `test/` — GoogleTest-based tests (binary: `mpadao_tests`)
- `benchmark/` — three categories:
  - `accuracy/` — Basel series convergence error measurement
  - `precision/` — ULP analysis and iterative precision loss
  - `performance/` — dot product time-to-solution

## Key Conventions

- **C++ Standard:** C++20 enforced (`CMAKE_CXX_STANDARD 20`)
- **CMake minimum:** 3.22
- **Compiler flags:** `-Wall -Wpedantic -Wno-narrowing -Wno-deprecated` (GCC/Clang); `-O3` only in Release builds; `/MP /Zc:__cplusplus` (MSVC)
- **Namespaces:** `sw::universal` for number types, `mpadao::` for version management, `sw::hprblas` for MTL4 extensions
- **Install prefix:** project source root (in-place: `bin/`, `lib/`, `include/`)
- **Template-heavy:** code uses extensive C++ templates for type-generic numeric algorithms
- **Boost dependency:** optional but used by solvers (multiprecision)

## CI/CD

GitHub Actions (`.github/workflows/cmake.yml`) builds on:
- Windows 2022 (MSVC), Ubuntu 22.04 (GCC), macOS 13 (Clang)
- Auto-installs Boost 1.81.0
- Runs `ctest` after build

## Docker Development

Single development container (`docker/Dockerfile`) based on Ubuntu 24.04 with GCC 14 and Clang 18. Build with `docker build -t stillwater/mpadao:latest docker/`. GCC 14 is the default; switch to Clang via `CC=clang-18 CXX=clang++-18 cmake ..`. DevContainer configured for `stillwater/mpadao:latest`.
