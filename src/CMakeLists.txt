cmake_minimum_required(VERSION 3.22)
project(mixedprecision_examples)

# example to build a library
add_subdirectory(lib/version)

# example to use a library and build a CLI command
add_subdirectory(tools/semver)

################################## Mixed-precision examples ################################################
# simple starter skeleton for projects that use the Universal Number System library
add_subdirectory(apps/mixed-precision/universal)

# a more complicated example project that consists of multiple source files
add_subdirectory(apps/mixed-precision/polynomial)

# a purposeful example of a mixed-precision experiment for the quadratic equation
add_subdirectory(apps/mixed-precision/quadratic)

# C++20 std::numbers vs Universal high-precision constants
add_subdirectory(apps/mixed-precision/constants)

################################## High-precision examples ################################################
# starter for projects that use the Universal Number System library adaptive precision facilities
add_subdirectory(apps/high-precision/digits_of_pi)



# Abseil logging integration (optional)
if(MPADAO_ENABLE_ABSEIL)
	add_subdirectory(apps/logging)
endif()

# a full blown mixed-precision algorithm development and optimization project targeting matrix inversion algorithms
if(MPADAO_BOOST_FOUND)
	add_subdirectory(solvers)
else()
	message(STATUS "Boost not found: skipping solvers (requires Boost multiprecision)")
endif()
