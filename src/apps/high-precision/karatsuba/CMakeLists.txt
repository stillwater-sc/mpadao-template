cmake_minimum_required(VERSION 3.22)
set(app_name karatsuba)
project(${app_name} CXX)

# MPADAO algorithm headers (binary splitting, karatsuba, etc.)
include_directories(${MPADAO_ROOT_DIR}/src/algorithms)

# source files that make up the command
set(SOURCE_FILES
	main.cpp
)

add_executable(${app_name} ${SOURCE_FILES})
set(folder "Applications/High-Precision")
set_target_properties(${app_name} PROPERTIES FOLDER ${folder})

install(TARGETS ${app_name} DESTINATION ${MPADAO_INSTALL_BIN_DIR})
