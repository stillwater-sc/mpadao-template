/** ************************************************************************
* Compute and display 50 digits of pi using ereal adaptive-precision type
*
* Uses Machin's formula: π/4 = 4·arctan(1/5) - arctan(1/239)
* with Taylor series for arctan and digit extraction via repeated
* multiply-by-10 and floor.
*
* @author:     Theodore Omtzigt
* @date:       2023-01-04
* @copyright:  Copyright (c) 2023 Stillwater Supercomputing, Inc.
* @license:    MIT Open Source license
*
* This file is part of the MPADAO project
* *************************************************************************
*/
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>

#include <universal/number/ereal/ereal.hpp>

namespace sw { namespace universal {

	// Compute arctan(x) for small x using Taylor series
	// arctan(x) = x - x^3/3 + x^5/5 - x^7/7 + ...
	template<unsigned nlimbs>
	ereal<nlimbs> compute_arctan_series(const ereal<nlimbs>& x, int terms = 100) {
		ereal<nlimbs> result(0.0);
		ereal<nlimbs> x_power = x;
		ereal<nlimbs> x_squared = x * x;
		ereal<nlimbs> one(1.0);

		for (int n = 0; n < terms; ++n) {
			// Use ereal division for full precision coefficient
			ereal<nlimbs> k_val(static_cast<double>(2 * n + 1));
			ereal<nlimbs> term = x_power / k_val;

			if (n % 2 == 0)
				result = result + term;
			else
				result = result - term;

			x_power = x_power * x_squared;
		}

		return result;
	}

	// Compute pi using Machin's formula: π/4 = 4·arctan(1/5) - arctan(1/239)
	template<unsigned nlimbs>
	ereal<nlimbs> compute_pi() {
		ereal<nlimbs> one(1.0);
		ereal<nlimbs> four(4.0);

		// arctan(1/5) — series converges fast, 50 terms is ample for 50 digits
		ereal<nlimbs> one_fifth = one / ereal<nlimbs>(5.0);
		ereal<nlimbs> arctan_1_5 = compute_arctan_series(one_fifth, 50);

		// arctan(1/239) — converges even faster, 30 terms sufficient
		ereal<nlimbs> one_over_239 = one / ereal<nlimbs>(239.0);
		ereal<nlimbs> arctan_1_239 = compute_arctan_series(one_over_239, 30);

		// π = 4 * (4·arctan(1/5) - arctan(1/239))
		ereal<nlimbs> pi_over_4 = four * arctan_1_5 - arctan_1_239;
		return four * pi_over_4;
	}

	// Extract decimal digits from an ereal value
	// Returns a string of digits (no decimal point)
	template<unsigned nlimbs>
	std::string extract_digits(ereal<nlimbs> value, int ndigits) {
		std::string digits;
		ereal<nlimbs> ten(10.0);

		for (int i = 0; i < ndigits; ++i) {
			ereal<nlimbs> f = floor(value);
			int digit = static_cast<int>(double(f));
			digits += static_cast<char>('0' + digit);
			value = (value - f) * ten;
		}

		return digits;
	}

}} // namespace sw::universal

int main()
try {
	using namespace sw::universal;

	// ereal<8> supports up to ~127 decimal digits, well beyond the 50 needed
	constexpr unsigned nlimbs = 8;
	constexpr int ndigits = 50;

	std::cout << "Computing " << ndigits << " digits of pi using ereal<" << nlimbs << ">\n";
	std::cout << "Algorithm: Machin's formula pi/4 = 4*arctan(1/5) - arctan(1/239)\n\n";

	ereal<nlimbs> pi = compute_pi<nlimbs>();

	// Extract integer part + fractional digits
	std::string all_digits = extract_digits(pi, ndigits + 1); // +1 for the "3" before decimal point

	std::string integer_part = all_digits.substr(0, 1);
	std::string fractional_part = all_digits.substr(1, ndigits);

	std::cout << "pi = " << integer_part << "." << fractional_part << "\n\n";

	// Verify against known reference
	const std::string reference = "14159265358979323846264338327950288419716939937510";
	std::cout << "Verification against known digits:\n";
	std::cout << "  computed:  " << fractional_part << "\n";
	std::cout << "  reference: " << reference << "\n";

	int matching = 0;
	for (int i = 0; i < ndigits && i < static_cast<int>(reference.size()); ++i) {
		if (fractional_part[i] == reference[i])
			++matching;
		else
			break;
	}

	std::cout << "  matching digits: " << matching << " / " << ndigits << "\n";

	if (matching == ndigits) {
		std::cout << "\nAll " << ndigits << " digits verified correctly!\n";
		return EXIT_SUCCESS;
	} else {
		std::cout << "\nFirst mismatch at digit " << (matching + 1) << "\n";
		return EXIT_FAILURE;
	}
}
catch(const char* msg) {
	std::cerr << "Caught unexpected exception: " << msg << std::endl;
	return EXIT_FAILURE;
}
catch(const std::exception& e) {
	std::cerr << "Caught exception: " << e.what() << std::endl;
	return EXIT_FAILURE;
}
