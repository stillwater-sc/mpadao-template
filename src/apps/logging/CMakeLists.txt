cmake_minimum_required(VERSION 3.22)
set(app_name logging)
project(${app_name} CXX)

# Abseil requires at least C++17
set(CMAKE_CXX_STANDARD 20)

# Process Abseil's CMake build system
set(ABSL_PROPAGATE_CXX_STD ON)
add_subdirectory(${MPADAO_ROOT_DIR}/ext/google/abseil build_abseil)

# Universal is a C++ header-only library, so we do not need to build anything
include_directories(${MPADAO_ROOT_DIR}/ext/stillwater-sc/universal/include)
include_directories(${MPADAO_ROOT_DIR}/ext/google/abseil)

# source files that make up the command
set(SOURCE_FILES
	main.cpp
)

add_executable(${app_name} ${SOURCE_FILES})
set(folder "Applications/Mixed-Precision")
set_target_properties(${app_name} PROPERTIES FOLDER ${folder})

# Declare dependency on the absl::log library
target_link_libraries(${app_name} absl::log absl::strings)
install(TARGETS ${app_name} DESTINATION ${MPADAO_INSTALL_BIN_DIR})
#install(FILES my-consolidated-include.hpp DESTINATION ${MPADAO_INSTALL_INCLUDE_DIR})
