cmake_minimum_required(VERSION 3.22)
project(benchmark_examples)

# example to benchmark accuracy
add_subdirectory(accuracy)

# example to benchmark precision
add_subdirectory(precision)

# example to benchmark performance (requires Boost for MTL4)
if(Boost_FOUND)
	add_subdirectory(performance)
else()
	message(STATUS "Boost not found: skipping performance benchmark (requires MTL4/Boost)")
endif()

