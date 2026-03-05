/** ************************************************************************
* Compute 300 digits of pi using the Chudnovsky formula with binary splitting
*
* The Chudnovsky formula converges at ~14.18 digits per term, making it
* vastly more efficient than Machin's arctan-based formula (~1.4 digits/term).
*
* Formula:
*   1/pi = 12 * sum_{k=0}^{inf} (-1)^k * (6k)! * (13591409 + 545140134*k)
*                                 / ((3k)! * (k!)^3 * 640320^(3k+3/2))
*
* Rearranged for binary splitting with P, Q, T terms:
*   P(k) = (6k-5)(2k-1)(6k-1)  for k>=1, P(0)=1
*   Q(k) = k^3 * C3_OVER_24    for k>=1, Q(0)=1
*   a(k) = (13591409 + 545140134*k) * (-1)^k
*
*   pi = (Q * C^(3/2)) / (12 * T)
*
* where C = 640320 and C3_OVER_24 = 640320^3 / 24 = 10939058860032000
*
* @author:     Theodore Omtzigt
* @date:       2025-01-15
* @copyright:  Copyright (c) 2025 Stillwater Supercomputing, Inc.
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

#include <binary_splitting.hpp>

namespace sw { namespace universal {

	// Extract decimal digits from an ereal value
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

	// ereal<8> supports up to ~127 decimal digits
	// (ereal<19> can reach ~303 digits but is significantly slower)
	constexpr unsigned nlimbs = 8;
	constexpr int ndigits = 100;
	using Real = ereal<nlimbs>;

	std::cout << "Computing " << ndigits << " digits of pi using ereal<" << nlimbs << ">\n";
	std::cout << "Algorithm: Chudnovsky formula with binary splitting\n";
	std::cout << "Convergence rate: ~14.18 digits per term\n\n";

	// Chudnovsky constant: C3_OVER_24 = 640320^3 / 24 = 10939058860032000
	// We compute this in high precision
	Real C(640320.0);
	Real C3_OVER_24 = C * C * C / Real(24.0);

	// Number of terms needed: ceil(ndigits / 14.18) + safety margin
	int nterms = static_cast<int>(ndigits / 14.0) + 2;

	int machin_terms = static_cast<int>(ndigits / 1.39) + 10;
	std::cout << "Using " << nterms << " terms (vs ~" << machin_terms << " needed by Machin's formula)\n\n";

	// Binary splitting PQT functions for the Chudnovsky series
	auto p_func = [](int k) -> Real {
		if (k == 0) return Real(1.0);
		Real k_val(static_cast<double>(k));
		Real six_k = Real(6.0) * k_val;
		Real two_k = Real(2.0) * k_val;
		// P(k) = (6k-5)(2k-1)(6k-1)
		return (six_k - Real(5.0)) * (two_k - Real(1.0)) * (six_k - Real(1.0));
	};

	auto q_func = [&C3_OVER_24](int k) -> Real {
		if (k == 0) return Real(1.0);
		Real k_val(static_cast<double>(k));
		// Q(k) = k^3 * C3_OVER_24
		return k_val * k_val * k_val * C3_OVER_24;
	};

	auto a_func = [](int k) -> Real {
		Real k_val(static_cast<double>(k));
		Real a_k = Real(13591409.0) + Real(545140134.0) * k_val;
		// (-1)^k
		if (k % 2 == 1) a_k = -a_k;
		return a_k;
	};

	// Run binary splitting over [0, nterms)
	auto result = mpadao::binary_split<Real>(0, nterms, p_func, q_func, a_func);

	// pi = (Q * C^(3/2)) / (12 * T)
	// C^(3/2) = C * sqrt(C) = 640320 * sqrt(640320)
	Real sqrt_C = sqrt(C);
	Real C_three_halves = C * sqrt_C;
	Real pi = (result.Q * C_three_halves) / (Real(12.0) * result.T);

	// Extract and display digits
	std::string all_digits = extract_digits(pi, ndigits + 1);
	std::string integer_part = all_digits.substr(0, 1);
	std::string fractional_part = all_digits.substr(1, ndigits);

	// Display in groups of 10 for readability
	std::cout << "pi = " << integer_part << ".\n";
	for (int i = 0; i < ndigits; i += 50) {
		std::cout << "  ";
		for (int j = i; j < std::min(i + 50, ndigits); j += 10) {
			int len = std::min(10, ndigits - j);
			std::cout << fractional_part.substr(j, len) << " ";
		}
		std::cout << "\n";
	}

	// 300+ reference digits of pi (fractional part)
	const std::string reference =
		"14159265358979323846264338327950288419716939937510"
		"58209749445923078164062862089986280348253421170679"
		"82148086513282306647093844609550582231725359408128"
		"48111745028410270193852110555964462294895493038196"
		"44288109756659334461284756482337867831652712019091"
		"45648566923460348610454326648213393607260249141273";

	// Verify
	std::cout << "\nVerification against " << reference.size() << " known digits:\n";
	int matching = 0;
	for (int i = 0; i < ndigits && i < static_cast<int>(reference.size()); ++i) {
		if (fractional_part[i] == reference[i])
			++matching;
		else
			break;
	}

	std::cout << "  matching digits: " << matching << " / " << ndigits << "\n";

	if (matching >= ndigits) {
		std::cout << "\nAll " << ndigits << " digits verified correctly!\n";
	} else {
		std::cout << "\nFirst mismatch at digit " << (matching + 1) << "\n";
		std::cout << "  computed:  " << fractional_part[matching] << "\n";
		std::cout << "  reference: " << reference[matching] << "\n";
	}

	// Comparison with Machin's formula
	std::cout << "\n--- Efficiency comparison ---\n";
	std::cout << "Chudnovsky: " << nterms << " terms for " << ndigits << " digits"
	          << " (~" << std::fixed << std::setprecision(1)
	          << static_cast<double>(ndigits) / nterms << " digits/term)\n";
	std::cout << "Machin:     ~" << machin_terms << " terms would be needed"
	          << " (~1.4 digits/term)\n";
	std::cout << "Speedup:    ~" << machin_terms / nterms << "x fewer terms\n";

	return (matching >= ndigits) ? EXIT_SUCCESS : EXIT_FAILURE;
}
catch(const char* msg) {
	std::cerr << "Caught unexpected exception: " << msg << std::endl;
	return EXIT_FAILURE;
}
catch(const std::exception& e) {
	std::cerr << "Caught exception: " << e.what() << std::endl;
	return EXIT_FAILURE;
}
