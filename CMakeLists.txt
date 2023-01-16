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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

	# include code quality flags
	set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wall -Wpedantic -Wno-narrowing -Wno-deprecated")
	# specific flags for debug and release builds
	#set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE} -O3 -DNDEBUG")
	#set(EXTRA_C_FLAGS_DEBUG "${EXTRA_C_FLAGS_DEBUG} -g3 -pthread")

elseif(MSVC)
	# include code quality flags
	set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} /MP")
#	add_definitions(-D _CRT_SECURE_NO_WARNINGS)
#	add_definitions(-D _SCL_SECURE_NO_WARNINGS)

    set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} /Zc:__cplusplus")

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

add_definitions(-D UNIVERSAL_ENABLE_TEST=OFF)

include_directories(${MPADAO_INSTALL_INCLUDE_DIR})
include_directories(${MTL4EXT_INCLUDE_HEADERS_DIR})
include_directories(${VERSION_INCLUDE_HEADERS_DIR})

enable_testing()
#include(CTest)

# Universal is a C++ header-only library, so we do not need to build anything
# if you do add this line, you will get all the Universal regression tests,
# playground, education, applications, and command line utilities.
# You can only add this build subdirectory to ONE and only ONE project
# within the IR workspace. Multiple includes will yield multiple definitions
# of build, install, and uninstall targets.
add_subdirectory(${MPADAO_ROOT_DIR}/ext/stillwater-sc/universal build_universal) 

# MTL4 is a C++ header-only library, so we do not need to build anything
add_subdirectory(${MPADAO_ROOT_DIR}/ext/stillwater-sc/mtl4 build_mtl4) 

add_subdirectory(src)
add_subdirectory(test)
