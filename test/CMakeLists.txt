cmake_minimum_required(VERSION 3.22)
set(app_name mpadao_tests)
project(${app_name} C CXX)

add_subdirectory(${MPIR_ROOT_DIR}/ext/google/googletest build_gtest) 

include_directories(${VERSION_INCLUDE_HEADERS_DIR})
include_directories(${PROJECT_SOURCE_DIR}/ext/google/googletest/include)

set(SOURCE_FILES main.cpp src/mpir_tests.cpp)

add_executable(${app_name} ${SOURCE_FILES})
set(folder "Google Tests")
set_target_properties(${app_name} PROPERTIES FOLDER ${folder})

target_link_libraries(${app_name} gtest)
install(TARGETS ${app_name} DESTINATION bin)
