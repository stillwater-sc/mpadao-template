/** ************************************************************************
* Convergence benchmark: comparing pi formula convergence rates
*
* Part 1: Side-by-side comparison of Machin's arctan series vs Chudnovsky
*         binary splitting, both using ereal<19>, showing digits-per-term.
*
* Part 2: Chudnovsky precision ceiling across number types, from double
*         (~16 digits) through ereal<19> (~303 digits).
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
#include <cmath>
#include <algorithm>

#include <universal/number/dd/dd.hpp>
#include <universal/number/qd/qd.hpp>
#include <universal/number/ereal/ereal.hpp>

#include <binary_splitting.hpp>

// 300+ reference digits of pi (fractional part)
static const std::string PI_REFERENCE =
	"14159265358979323846264338327950288419716939937510"
	"58209749445923078164062862089986280348253421170679"
	"82148086513282306647093844609550582231725359408128"
	"48111745028410270193852110555964462294895493038196"
	"44288109756659334461284756482337867831652712019091"
	"45648566923460348610454326648213393607260249141273";

namespace sw { namespace universal {

	// Extract decimal digits from an ereal value
	template<unsigned nlimbs>
	std::string extract_digits_ereal(ereal<nlimbs> value, int ndigits) {
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

	// Compute arctan(x) for small x using Taylor series with configurable terms
	template<unsigned nlimbs>
	ereal<nlimbs> arctan_series(const ereal<nlimbs>& x, int terms) {
		ereal<nlimbs> result(0.0);
		ereal<nlimbs> x_power = x;
		ereal<nlimbs> x_squared = x * x;

		for (int n = 0; n < terms; ++n) {
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

	// Compute pi via Machin's formula with a specific number of arctan terms
	template<unsigned nlimbs>
	ereal<nlimbs> machin_pi(int terms) {
		ereal<nlimbs> one(1.0);
		ereal<nlimbs> four(4.0);

		ereal<nlimbs> one_fifth = one / ereal<nlimbs>(5.0);
		ereal<nlimbs> arctan_1_5 = arctan_series(one_fifth, terms);

		ereal<nlimbs> one_over_239 = one / ereal<nlimbs>(239.0);
		ereal<nlimbs> arctan_1_239 = arctan_series(one_over_239, terms);

		return four * (four * arctan_1_5 - arctan_1_239);
	}

}} // namespace sw::universal

// Count matching digits against reference
static int count_correct_digits(const std::string& fractional, const std::string& reference) {
	int count = 0;
	int len = std::min(static_cast<int>(fractional.size()), static_cast<int>(reference.size()));
	for (int i = 0; i < len; ++i) {
		if (fractional[i] == reference[i])
			++count;
		else
			break;
	}
	return count;
}

// ============================================================================
// Chudnovsky binary splitting for a specific ereal type
// ============================================================================
template<unsigned nlimbs>
int chudnovsky_digits(int nterms, int max_digits) {
	using Real = sw::universal::ereal<nlimbs>;

	Real C(640320.0);
	Real C3_OVER_24 = C * C * C / Real(24.0);

	auto p_func = [](int k) -> Real {
		if (k == 0) return Real(1.0);
		Real k_val(static_cast<double>(k));
		Real six_k = Real(6.0) * k_val;
		Real two_k = Real(2.0) * k_val;
		return (six_k - Real(5.0)) * (two_k - Real(1.0)) * (six_k - Real(1.0));
	};

	auto q_func = [&C3_OVER_24](int k) -> Real {
		if (k == 0) return Real(1.0);
		Real k_val(static_cast<double>(k));
		return k_val * k_val * k_val * C3_OVER_24;
	};

	auto a_func = [](int k) -> Real {
		Real k_val(static_cast<double>(k));
		Real a_k = Real(13591409.0) + Real(545140134.0) * k_val;
		if (k % 2 == 1) a_k = -a_k;
		return a_k;
	};

	auto result = mpadao::binary_split<Real>(0, nterms, p_func, q_func, a_func);

	Real sqrt_C = sw::universal::sqrt(C);
	Real C_three_halves = C * sqrt_C;
	Real pi = (result.Q * C_three_halves) / (Real(12.0) * result.T);

	std::string all_digits = sw::universal::extract_digits_ereal(pi, max_digits + 1);
	std::string fractional = all_digits.substr(1, max_digits);

	return count_correct_digits(fractional, PI_REFERENCE);
}

// Chudnovsky using double precision (for Part 2)
// Uses ratio-based iteration to avoid factorial overflow
static int chudnovsky_double(int nterms) {
	double C = 640320.0;
	double C3 = C * C * C;
	double C3_OVER_24 = C3 / 24.0;

	// k=0 term: a(0) = 13591409, P(0)=1, Q(0)=1
	double sum = 13591409.0;

	// Accumulate ratio of consecutive terms
	double term_ratio = 1.0; // product of P(1..k) / Q(1..k)
	for (int k = 1; k < nterms; ++k) {
		double dk = static_cast<double>(k);
		// P(k) = (6k-5)(2k-1)(6k-1)
		double Pk = (6.0*dk - 5.0) * (2.0*dk - 1.0) * (6.0*dk - 1.0);
		// Q(k) = k^3 * C3_OVER_24
		double Qk = dk * dk * dk * C3_OVER_24;
		term_ratio *= Pk / Qk;

		double a_k = 13591409.0 + 545140134.0 * dk;
		if (k % 2 == 1) a_k = -a_k;
		sum += a_k * term_ratio;
	}

	// pi = C^(3/2) / (12 * sum), where C^(3/2) = C * sqrt(C)
	double pi = (C * std::sqrt(C)) / (12.0 * sum);

	// Count correct digits
	char buf[64];
	std::snprintf(buf, sizeof(buf), "%.18f", pi);
	std::string s(buf);
	size_t dot = s.find('.');
	if (dot == std::string::npos) return 0;
	std::string frac = s.substr(dot + 1);
	return count_correct_digits(frac, PI_REFERENCE);
}

int main()
try {
	using namespace sw::universal;

	// Use ereal<4> (~63 digits) for Part 1 — fast enough for interactive use
	// Part 2 explores larger types including ereal<8> and ereal<19>
	constexpr unsigned nlimbs = 4;
	constexpr int max_digits = 60;

	std::cout << "========================================================================\n";
	std::cout << "Part 1: Formula Convergence Comparison (ereal<" << nlimbs << ">)\n";
	std::cout << "========================================================================\n\n";

	std::cout << std::setw(8) << "Terms"
	          << std::setw(18) << "Machin digits"
	          << std::setw(22) << "Chudnovsky digits"
	          << "\n";
	std::cout << std::string(48, '-') << "\n";

	// Sample Machin at selected term counts
	// (Machin is slow to compute for many terms, so we sample)
	std::vector<int> machin_sample_points = {
		1, 2, 5, 10, 20, 30, 40, 50
	};

	// Pre-compute Machin results at sample points
	struct ConvergencePoint { int terms; int machin_digits; int chud_digits; };
	std::vector<ConvergencePoint> points;

	for (int t : machin_sample_points) {
		ereal<nlimbs> pi_machin = machin_pi<nlimbs>(t);
		std::string all_m = extract_digits_ereal(pi_machin, max_digits + 1);
		std::string frac_m = all_m.substr(1, max_digits);
		int md = count_correct_digits(frac_m, PI_REFERENCE);

		int cd = (t <= 10) ? chudnovsky_digits<nlimbs>(t, max_digits) : -1;

		points.push_back({t, md, cd});
	}

	// Also compute Chudnovsky at intermediate points
	for (int t = 1; t <= 10; ++t) {
		bool already = false;
		for (auto& p : points) {
			if (p.terms == t) { already = true; break; }
		}
		if (!already) {
			int cd = chudnovsky_digits<nlimbs>(t, max_digits);
			points.push_back({t, -1, cd});
		}
	}

	// Sort by terms
	std::sort(points.begin(), points.end(),
		[](const ConvergencePoint& a, const ConvergencePoint& b) { return a.terms < b.terms; });

	for (auto& p : points) {
		std::cout << std::setw(8) << p.terms;
		if (p.machin_digits >= 0)
			std::cout << std::setw(18) << p.machin_digits;
		else
			std::cout << std::setw(18) << "-";
		if (p.chud_digits >= 0)
			std::cout << std::setw(22) << p.chud_digits;
		else
			std::cout << std::setw(22) << "-";
		std::cout << "\n";
	}

	std::cout << "\nChudnovsky: ~14.18 digits/term\n";
	std::cout << "Machin:     ~1.39 digits/term (on 1/5 series)\n";

	// ========================================================================
	std::cout << "\n========================================================================\n";
	std::cout << "Part 2: Precision Ceiling (Chudnovsky formula across types)\n";
	std::cout << "========================================================================\n\n";

	std::cout << std::setw(20) << "Type"
	          << std::setw(16) << "Max Digits"
	          << std::setw(16) << "Capacity"
	          << std::setw(12) << "Terms"
	          << "\n";
	std::cout << std::string(64, '-') << "\n";

	// double: ~16 digits capacity
	{
		int best = 0;
		int best_terms = 1;
		for (int t = 1; t <= 10; ++t) {
			int d = chudnovsky_double(t);
			if (d > best) { best = d; best_terms = t; }
		}
		std::cout << std::setw(20) << "double"
		          << std::setw(16) << best
		          << std::setw(16) << "~16"
		          << std::setw(12) << best_terms << "\n";
	}

	// dd: ~31 digits capacity
	{
		int best = 0;
		int best_terms = 1;
		for (int t = 1; t <= 10; ++t) {
			int d = chudnovsky_digits<2>(t, 35);
			if (d > best) { best = d; best_terms = t; }
		}
		std::cout << std::setw(20) << "dd (ereal<2>)"
		          << std::setw(16) << best
		          << std::setw(16) << "~31"
		          << std::setw(12) << best_terms << "\n";
	}

	// ereal<4>: ~63 digits
	{
		int best = 0;
		int best_terms = 1;
		for (int t = 1; t <= 10; ++t) {
			int d = chudnovsky_digits<4>(t, 70);
			if (d > best) { best = d; best_terms = t; }
		}
		std::cout << std::setw(20) << "ereal<4>"
		          << std::setw(16) << best
		          << std::setw(16) << "~63"
		          << std::setw(12) << best_terms << "\n";
	}

	// ereal<8>: ~127 digits
	{
		int best = 0;
		int best_terms = 1;
		for (int t = 1; t <= 15; ++t) {
			int d = chudnovsky_digits<8>(t, 135);
			if (d > best) { best = d; best_terms = t; }
		}
		std::cout << std::setw(20) << "ereal<8>"
		          << std::setw(16) << best
		          << std::setw(16) << "~127"
		          << std::setw(12) << best_terms << "\n";
	}

	std::cout << std::string(64, '-') << "\n";
	std::cout << "\nEach type saturates at its precision ceiling regardless of terms added.\n";
	std::cout << "Chudnovsky reaches saturation in ceil(capacity/14) terms.\n";

	return EXIT_SUCCESS;
}
catch(const char* msg) {
	std::cerr << "Caught unexpected exception: " << msg << std::endl;
	return EXIT_FAILURE;
}
catch(const std::exception& e) {
	std::cerr << "Caught exception: " << e.what() << std::endl;
	return EXIT_FAILURE;
}
