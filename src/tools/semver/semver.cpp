#include "mpadao.hpp"

static const std::string HEADER = "Mixed-Precision Algorithm Development and Optimization with the Universal Numbers Library";

int main(int argc, char* argv[])
try {
	std::cout << HEADER << '\n';

	std::cout << "VERSION: " << ir::get_semver() << '\n';
}
catch(...) {
	std::cerr << "Caught unexpected exception" << std::endl;
}

