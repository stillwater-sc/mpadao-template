cmake_minimum_required(VERSION 3.22)
set(app_name mpadao_tests)
project(${app_name} C CXX)

add_subdirectory(${MPADAO_ROOT_DIR}/ext/google/googletest build_gtest)

include_directories(${VERSION_INCLUDE_HEADERS_DIR})
include_directories(${MPADAO_ROOT_DIR}/ext/stillwater-sc/universal/include)
include_directories(${MPADAO_ROOT_DIR}/ext/google/googletest/googletest/include)

set(SOURCE_FILES main.cpp src/mpadao_tests.cpp)

add_executable(${app_name} ${SOURCE_FILES})
set(folder "Google Tests")
set_target_properties(${app_name} PROPERTIES FOLDER ${folder})

target_link_libraries(${app_name} gtest versionLib)
add_test(NAME ${app_name} COMMAND $<TARGET_FILE:${app_name}>)
install(TARGETS ${app_name} DESTINATION ${MPADAO_INSTALL_BIN_DIR})
