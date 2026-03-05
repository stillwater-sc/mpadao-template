cmake_minimum_required(VERSION 3.22)
set(app_name convergence_benchmark)
project(${app_name} CXX)

# Universal is a C++ header-only library, so we do not need to build anything
include_directories(${MPADAO_ROOT_DIR}/ext/stillwater-sc/universal/include/sw)

# MPADAO algorithm headers (binary splitting, karatsuba, etc.)
include_directories(${MPADAO_ROOT_DIR}/src/algorithms)

# source files that make up the command
set(SOURCE_FILES
	main.cpp
)

add_executable(${app_name} ${SOURCE_FILES})
set(folder "Benchmarks/Convergence")
set_target_properties(${app_name} PROPERTIES FOLDER ${folder})

install(TARGETS ${app_name} DESTINATION ${MPADAO_INSTALL_BIN_DIR})
