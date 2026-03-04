cmake_minimum_required(VERSION 3.22)
set(app_name constants)
project(${app_name} CXX)

# Universal is a C++ header-only library, so we do not need to build anything
include_directories(${MPADAO_ROOT_DIR}/ext/stillwater-sc/universal/include/sw)

set(SOURCE_FILES
	constants.cpp
)

add_executable(${app_name} ${SOURCE_FILES})
set(folder "Applications/Mixed-Precision")
set_target_properties(${app_name} PROPERTIES FOLDER ${folder})
set(test_name ${app_name})
add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${app_name}>)

install(TARGETS ${app_name} DESTINATION ${MPADAO_INSTALL_BIN_DIR})
