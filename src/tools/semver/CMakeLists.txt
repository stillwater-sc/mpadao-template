cmake_minimum_required(VERSION 3.22)
set(app_name semver)
project(${app_name})

# semver CLI command
set(SOURCE_FILES semver.cpp)
add_executable(${app_name} ${SOURCE_FILES})
set(folder "Tools")
set_target_properties(${app_name} PROPERTIES FOLDER ${folder})

target_link_libraries(${app_name} versionLib)
install(TARGETS ${app_name} DESTINATION ${MPADAO_INSTALL_BIN_DIR})
