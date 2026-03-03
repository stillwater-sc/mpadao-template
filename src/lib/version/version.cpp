#include <version.hpp>

namespace mpadao {

std::string get_semver() {
	return std::to_string(MAJOR_VERSION) + "." + std::to_string(MINOR_VERSION) + "." + std::to_string(PATCH_VERSION);
}

} // namespace mpadao
