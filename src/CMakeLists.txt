cmake_minimum_required(VERSION 3.22)
project(mixedprecision_examples)

# example to build a library
add_subdirectory(lib/version)

# example to use a library and build a CLI command
add_subdirectory(tools/semver)

# simple starter skeleton for projects that use the Universal Number System library
add_subdirectory(apps/universal)

# a more complicated example project that consists of multiple source files
add_subdirectory(apps/polynomial)

# a purposefull example of a mixed-precision experiment for the quadratic equation
add_subdirectory(apps/quadratic)

# a purposefull example of using Abseil 
add_subdirectory(apps/logging)

# a full blown mixed-precision algorithm development and optimization project targeting matrix inversion algorithms
add_subdirectory(solvers)
