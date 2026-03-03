cmake_minimum_required(VERSION 3.22)

if(NOT DEFINED MPADAO_MAJOR)
	set(MPADAO_MAJOR 0)
endif()
if(NOT DEFINED MPADAO_MINOR)
	set(MPADAO_MINOR 8)
endif()
if(NOT DEFINED MPADAO_PATCH)
	set(MPADAO_PATCH 1)
endif()
project(mpadao-template
	DESCRIPTION "High-Performance Mixed-Precision Algorithm Development and Optimization Environment"
	VERSION "${MPADAO_MAJOR}.${MPADAO_MINOR}.${MPADAO_PATCH}"
	LANGUAGES C CXX
	HOMEPAGE_URL "https://github.com/stillwater-sc/mpadao-template")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "C++20 has been enabled by default")

# Compiler specific environments
if(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

	# include code quality flags
	set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wall -Wpedantic -Wno-narrowing -Wno-deprecated")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_C_FLAGS}")

elseif(MSVC)
	# include code quality flags
	set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} /MP")
#	add_definitions(-D _CRT_SECURE_NO_WARNINGS)
#	add_definitions(-D _SCL_SECURE_NO_WARNINGS)

    set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} /Zc:__cplusplus")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_C_FLAGS}")

endif()

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

find_package(Boost)
if(Boost_FOUND)
	message(STATUS "Boost_INCLUDE_DIR" ${Boost_INCLUDE_DIR})
	include_directories(${Boost_INCLUDE_DIR})
endif(Boost_FOUND)

set(MPADAO_ROOT_DIR ${PROJECT_SOURCE_DIR})
set(MPADAO_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(MPADAO_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(MPADAO_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

# constituent library headers
set(MTL4EXT_INCLUDE_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/mtl4ext)
set(VERSION_INCLUDE_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/lib/version)

set(UNIVERSAL_ENABLE_TEST OFF CACHE BOOL "Disable Universal regression tests" FORCE)

option(MPADAO_ENABLE_ABSEIL "Enable Abseil logging support" OFF)

include_directories(${MPADAO_INSTALL_INCLUDE_DIR})
include_directories(${MTL4EXT_INCLUDE_HEADERS_DIR})
include_directories(${VERSION_INCLUDE_HEADERS_DIR})

# Generate version configuration header from CMake variables
configure_file(
	${PROJECT_SOURCE_DIR}/src/lib/version/version_config.hpp.in
	${PROJECT_BINARY_DIR}/generated/version_config.hpp
)
include_directories(${PROJECT_BINARY_DIR}/generated)

enable_testing()
#include(CTest)

# Universal is a C++ header-only library, so we do not need to build anything
# if you do add this line, you will get all the Universal regression tests,
# playground, education, applications, and command line utilities.
# You can only add this build subdirectory to ONE and only ONE project
# within the MPADAO workspace. Multiple includes will yield multiple definitions
# of build, install, and uninstall targets.
add_subdirectory(${MPADAO_ROOT_DIR}/ext/stillwater-sc/universal build_universal)

# MTL4 is a C++ header-only library that requires Boost
if(Boost_FOUND)
	add_subdirectory(${MPADAO_ROOT_DIR}/ext/stillwater-sc/mtl4 build_mtl4)
else()
	message(STATUS "Boost not found: skipping MTL4 (required by solvers and performance benchmark)")
endif()

add_subdirectory(test)
add_subdirectory(src)
add_subdirectory(benchmark)
