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

# Boost is only needed for solvers (boost::multiprecision::cpp_bin_float_quad).
# We only use header-only Boost libraries, so no built Boost is required.
#
# Detection strategy:
#  1. Try find_package in CONFIG mode (installed Boost >= 1.70 with CMake config)
#  2. Fall back to BOOST_ROOT pointing to a Boost source/include tree
#     Set BOOST_ROOT as a CMake variable (-DBOOST_ROOT=...) or environment variable.
#
# CMake 4.0 removed the built-in FindBoost module (CMP0167).
# CMP0144: find_package honors upper-case <PACKAGENAME>_ROOT variables (CMake 3.27+).
if(POLICY CMP0167)
	cmake_policy(SET CMP0167 NEW)
endif()
if(POLICY CMP0144)
	cmake_policy(SET CMP0144 NEW)
endif()
set(MPADAO_BOOST_FOUND FALSE)

# Expose BOOST_ROOT in the CMake GUI so users can browse to their Boost tree.
# The CACHE variable persists across configures and won't be cleared by find_package.
set(BOOST_ROOT "${BOOST_ROOT}" CACHE PATH "Path to Boost source/install tree (e.g. C:/local/boost_1_86_0)")

# First, try config-mode (works with installed Boost >= 1.70)
find_package(Boost QUIET CONFIG)
if(Boost_FOUND)
	set(MPADAO_BOOST_FOUND TRUE)
	message(STATUS "Boost found via CMake config: ${Boost_INCLUDE_DIRS}")
	include_directories(${Boost_INCLUDE_DIRS})
else()
	# Collect candidate paths from CMake variable, environment variable, and GUI cache
	set(_boost_candidates)
	if(BOOST_ROOT)
		list(APPEND _boost_candidates "${BOOST_ROOT}")
	endif()
	if(DEFINED ENV{BOOST_ROOT})
		list(APPEND _boost_candidates "$ENV{BOOST_ROOT}")
	endif()

	foreach(_candidate IN LISTS _boost_candidates)
		if(EXISTS "${_candidate}/boost/version.hpp")
			set(MPADAO_BOOST_FOUND TRUE)
			message(STATUS "Boost found via source tree: ${_candidate}")
			include_directories("${_candidate}")
			break()
		endif()
	endforeach()

	if(NOT MPADAO_BOOST_FOUND)
		message(STATUS "Boost not found: set BOOST_ROOT to your Boost source tree to enable solvers")
		message(STATUS "  cmake -DBOOST_ROOT=/path/to/boost_1_86_0 ..")
	endif()
endif()

set(MPADAO_ROOT_DIR ${PROJECT_SOURCE_DIR})
set(MPADAO_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(MPADAO_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(MPADAO_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

# constituent library headers
set(MTLEXT_INCLUDE_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/mtlext)
set(VERSION_INCLUDE_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/lib/version)

set(UNIVERSAL_ENABLE_TEST OFF CACHE BOOL "Disable Universal regression tests" FORCE)
set(MTL5_BUILD_TESTS OFF CACHE BOOL "Disable MTL5 tests when building as submodule" FORCE)
set(MTL5_BUILD_EXAMPLES OFF CACHE BOOL "Disable MTL5 examples when building as submodule" FORCE)

option(MPADAO_ENABLE_ABSEIL "Enable Abseil logging support" OFF)

include_directories(${MPADAO_INSTALL_INCLUDE_DIR})
include_directories(${MTLEXT_INCLUDE_HEADERS_DIR})
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

# MTL5 is a C++ header-only library (no Boost dependency)
add_subdirectory(${MPADAO_ROOT_DIR}/ext/stillwater-sc/mtl5 build_mtl5)
# MTL5 headers: source tree + generated version.hpp in build tree
include_directories(${MPADAO_ROOT_DIR}/ext/stillwater-sc/mtl5/include)
include_directories(${CMAKE_BINARY_DIR}/build_mtl5/include)

add_subdirectory(test)
add_subdirectory(src)
add_subdirectory(benchmark)
