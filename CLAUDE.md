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
- `stillwater-sc/mtl5` — C++20 header-only matrix/linear algebra library (no Boost dependency)
- `google/googletest` — test framework
- `google/abseil` — logging and utilities (C++20 required, optional via `MPADAO_ENABLE_ABSEIL`)

**Source layout (`src/`):**
- `apps/` — example applications demonstrating mixed-precision concepts
  - `universal/` — basic Universal number system usage (cfloat ops, ULP calculations)
  - `polynomial/` — polynomial evaluation across multiple number types (integers, fixpnts, areals, cfloats, posits, logs, unums)
  - `quadratic/` — catastrophic cancellation in quadratic formula (educational)
  - `logging/` — Abseil logging integration (only built when `MPADAO_ENABLE_ABSEIL=ON`)
- `solvers/` — production-like algorithms (Gauss-Jordan matrix inversion using MTL5 + Boost multiprecision)
- `mtlext/` — MTL5 extension headers (matrix generators, vector utils, norms)
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
- **Namespaces:** `sw::universal` for number types, `mpadao::` for version management, `sw::hprblas` for MTL5 extensions
- **Install prefix:** project source root (in-place: `bin/`, `lib/`, `include/`)
- **Template-heavy:** code uses extensive C++ templates for type-generic numeric algorithms
- **Boost dependency:** optional, used only by solvers (`boost::multiprecision::cpp_bin_float_quad`); MTL5 has no Boost dependency

## CI/CD

GitHub Actions (`.github/workflows/cmake.yml`) builds on:
- Windows 2022 (MSVC), Ubuntu 22.04 (GCC), macOS 15 (Clang)
- Auto-installs Boost 1.81.0
- Runs `ctest` after build

## Docker Development

Single development container (`docker/Dockerfile`) based on Ubuntu 24.04 with GCC 14 and Clang 18. Build with `docker build -t stillwater/mpadao:latest docker/`. GCC 14 is the default; switch to Clang via `CC=clang-18 CXX=clang++-18 cmake ..`. DevContainer configured for `stillwater/mpadao:latest`.

## Common Development Tasks

When adding a new application:
1. Create directory under src/apps/<category>/<name>/
2. Follow the CMakeLists.txt pattern from digits_of_pi (see scaffold-app skill)
3. Add add_subdirectory() to src/CMakeLists.txt in the appropriate section
4. Build and verify before committing

When adding a new benchmark:
1. Create directory under benchmark/<name>/
2. Follow the CMakeLists.txt pattern from benchmark/accuracy/
3. Add add_subdirectory() to benchmark/CMakeLists.txt

## Development Plans

Project plans live in `.claude/plans/` and are checked into git for team visibility.

**Naming:** Use descriptive kebab-case names (e.g., `karatsuba-multiplication.md`, `mtl4-to-mtl5-migration.md`). Never commit auto-generated slug names.

**Lifecycle:**
1. Develop the plan using `/plan` (auto-saved to `~/.claude/plans/` with a random name)
2. When ready to share, copy it to `.claude/plans/` with a human-readable name
3. Commit it alongside the code
4. Delete from `.claude/plans/` once the work is merged to `main`

**Format:** Each plan should include `## Context`, `## Changes`, `## Execution Order`, and `## Verification` sections.

## Branch Strategy

- `dev` -> `integration` -> `main` (three-tier)
- Feature branches from `dev`
- PRs target `integration` or `main`
- CI triggers on push to dev/integration/main and PRs to integration/main

## Testing Expectations

- All new apps should produce verifiable output (reference values, assertions)
- Benchmarks should include hardcoded reference data for validation
- Run ctest before committing to verify no regressions

## Known Constraints

- ereal<19> arithmetic is very slow (~minutes for 300-digit computations).
  Use ereal<8> (127 digits) for interactive demos.
- CI does not install Boost — solvers/ is not exercised in CI.
- Version is managed centrally in root CMakeLists.txt
  (MPADAO_MAJOR/MINOR/PATCH) and propagated via configure_file.
