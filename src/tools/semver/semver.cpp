#include "mpadao.hpp"

static const std::string HEADER = "Mixed-Precision Algorithm Development and Optimization with the Universal Numbers Library";

int main(int argc, char* argv[])
try {
	std::cout << HEADER << '\n';

	std::cout << "VERSION: " << mpadao::get_semver() << '\n';

	return EXIT_SUCCESS;
}
catch(...) {
	std::cerr << "Caught unexpected exception" << std::endl;
	return EXIT_FAILURE;
}
