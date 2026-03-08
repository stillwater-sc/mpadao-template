---
name: scaffold-app
description: Scaffold a new MPADAO app or benchmark with CMakeLists.txt boilerplate
argument-hint: "<category> <name> — e.g., 'high-precision fft' or 'benchmark memory'"
disable-model-invocation: true
---

Create a new MPADAO application or benchmark. Parse $ARGUMENTS to determine placement:

If first argument is "benchmark":
  - Create in benchmark/<name>/
  - Follow the CMakeLists.txt pattern from benchmark/accuracy/
  - Add add_subdirectory() to benchmark/CMakeLists.txt
  - Set folder to "Benchmarks/<Name>"

Otherwise:
  - Create in src/apps/<category>/<name>/
  - Follow the CMakeLists.txt pattern from src/apps/high-precision/digits_of_pi/
  - Add add_subdirectory() to src/CMakeLists.txt
  - Set folder to "Applications/<Category>"

For all targets:
1. Create the directory
2. Create CMakeLists.txt with:
   - cmake_minimum_required(VERSION 3.22)
   - Appropriate app_name, include paths, folder property
   - install(TARGETS ...) line
3. Create main.cpp with:
   - Standard copyright header (Stillwater Supercomputing, Inc., MIT license)
   - try/catch main() pattern matching existing apps
   - Appropriate Universal/MTL5 includes based on the app's purpose
4. Wire up the parent CMakeLists.txt
5. Build to verify: cd build && cmake .. && make <target> -j$(nproc)
