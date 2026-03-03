#pragma once
#include <string>
#include <version_config.hpp>

namespace mpadao {

inline constexpr int MAJOR_VERSION = MPADAO_VERSION_MAJOR;
inline constexpr int MINOR_VERSION = MPADAO_VERSION_MINOR;
inline constexpr int PATCH_VERSION = MPADAO_VERSION_PATCH;

std::string get_semver();

} // namespace mpadao
