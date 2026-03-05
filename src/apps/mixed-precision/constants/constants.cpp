/** ************************************************************************
* constants.cpp: C++20 std::numbers vs Universal high-precision constants
*
* C++20 introduced <numbers> with mathematical constants like
* std::numbers::pi, std::numbers::e, and std::numbers::ln2.
* These are welcome — they replace the non-standard M_PI macro —
* but they are limited to float, double, and long double precision.
*
* For many scientific and engineering applications, double precision
* (~15.9 decimal digits) is not enough.  The Universal library provides
* the same constants in double-double (~31 digits), triple-double
* (~48 digits), and quad-double (~62 digits) precision using both
* the classic dd/qd types and the new cascade (floatcascade) types.
*
* This program juxtaposes the two facilities:
*
*   Part 1 — What C++20 gives you: std::numbers
*   Part 2 — What Universal adds: dd, qd, and cascade constants
*   Part 3 — When the extra digits matter: a concrete computation
*
* @author:     Theodore Omtzigt
* @date:       2026-03-04
* @copyright:  Copyright (c) 2026 Stillwater Supercomputing, Inc.
* @license:    MIT Open Source license
*
* This file is part of the MPADAO project
* *************************************************************************
*/
#include <universal/utility/directives.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <numbers>    // C++20 standard constants

// Universal number systems with pre-computed high-precision constants
#include <universal/number/dd/dd.hpp>
#include <universal/number/qd/qd.hpp>
#include <universal/number/dd_cascade/dd_cascade.hpp>
#include <universal/number/td_cascade/td_cascade.hpp>
#include <universal/number/qd_cascade/qd_cascade.hpp>

// =====================================================================
//  Helpers
// =====================================================================

void Separator(const std::string& title) {
	std::cout << "\n" << std::string(70, '=') << "\n"
	          << title << "\n"
	          << std::string(70, '=') << "\n\n";
}

// Show a constant at a given precision, with its type name
template<typename T>
void ShowConstant(const std::string& label, const T& value, int digits) {
	std::cout << "  " << std::setw(24) << std::left << label
	          << std::setprecision(digits) << std::right << value << '\n';
}

// =====================================================================
//  main
// =====================================================================

int main()
try {
	using namespace sw::universal;

	// =================================================================
	//  Part 1 — C++20 std::numbers
	// =================================================================

	Separator("PART 1:  C++20 <numbers> — the standard constants");

	std::cout << "C++20 provides mathematical constants as constexpr values\n"
	          << "in the std::numbers namespace.  These replace the non-standard\n"
	          << "M_PI, M_E, M_LN2 macros from <cmath>.\n\n"
	          << "Available precisions: float, double, long double.\n\n";

	std::cout << "  --- pi ---\n";
	ShowConstant("std::numbers::pi_v<float>",       std::numbers::pi_v<float>, 9);
	ShowConstant("std::numbers::pi_v<double>",      std::numbers::pi_v<double>, 17);
	ShowConstant("std::numbers::pi_v<long double>", std::numbers::pi_v<long double>, 21);

	std::cout << "\n  --- e ---\n";
	ShowConstant("std::numbers::e_v<float>",        std::numbers::e_v<float>, 9);
	ShowConstant("std::numbers::e_v<double>",       std::numbers::e_v<double>, 17);
	ShowConstant("std::numbers::e_v<long double>",  std::numbers::e_v<long double>, 21);

	std::cout << "\n  --- ln(2) ---\n";
	ShowConstant("std::numbers::ln2_v<float>",       std::numbers::ln2_v<float>, 9);
	ShowConstant("std::numbers::ln2_v<double>",      std::numbers::ln2_v<double>, 17);
	ShowConstant("std::numbers::ln2_v<long double>", std::numbers::ln2_v<long double>, 21);

	std::cout << "\n  --- sqrt(2) ---\n";
	ShowConstant("std::numbers::sqrt2_v<float>",       std::numbers::sqrt2_v<float>, 9);
	ShowConstant("std::numbers::sqrt2_v<double>",      std::numbers::sqrt2_v<double>, 17);
	ShowConstant("std::numbers::sqrt2_v<long double>", std::numbers::sqrt2_v<long double>, 21);

	std::cout << "\n  --- phi (golden ratio) ---\n";
	ShowConstant("std::numbers::phi_v<float>",       std::numbers::phi_v<float>, 9);
	ShowConstant("std::numbers::phi_v<double>",      std::numbers::phi_v<double>, 17);
	ShowConstant("std::numbers::phi_v<long double>", std::numbers::phi_v<long double>, 21);

	std::cout << "\n  Limitation: long double is the ceiling.  On most x86-64 Linux\n"
	          << "  systems long double is 80-bit extended (~18.5 decimal digits).\n"
	          << "  On MSVC, long double == double (no extra precision).\n";

	// =================================================================
	//  Part 2 — Universal high-precision constants
	// =================================================================

	Separator("PART 2:  Universal high-precision constants");

	std::cout << "Universal provides pre-computed constants at higher precisions:\n\n"
	          << "  double-double (dd)        ~31 decimal digits   (2 doubles)\n"
	          << "  dd_cascade                ~31 decimal digits   (floatcascade<2>)\n"
	          << "  td_cascade                ~48 decimal digits   (floatcascade<3>)\n"
	          << "  quad-double (qd)          ~62 decimal digits   (4 doubles)\n"
	          << "  qd_cascade                ~62 decimal digits   (floatcascade<4>)\n\n";

	// --- pi ---
	std::cout << "  --- pi ---\n";
	ShowConstant("std::numbers::pi",   std::numbers::pi, 17);
	ShowConstant("dd_pi",              dd_pi, 33);
	ShowConstant("ddc_pi",             ddc_pi, 33);
	ShowConstant("tdc_pi",             tdc_pi, 50);
	ShowConstant("qd_pi",             qd_pi, 65);
	ShowConstant("qdc_pi",            qdc_pi, 65);

	// --- e ---
	std::cout << "\n  --- e ---\n";
	ShowConstant("std::numbers::e",    std::numbers::e, 17);
	ShowConstant("dd_e",               dd_e, 33);
	ShowConstant("ddc_e",              ddc_e, 33);
	ShowConstant("tdc_e",              tdc_e, 50);
	ShowConstant("qd_e",              qd_e, 65);
	ShowConstant("qdc_e",             qdc_e, 65);

	// --- ln(2) ---
	std::cout << "\n  --- ln(2) ---\n";
	ShowConstant("std::numbers::ln2",  std::numbers::ln2, 17);
	ShowConstant("dd_ln2",             dd_ln2, 33);
	ShowConstant("ddc_ln2",            ddc_ln2, 33);
	ShowConstant("tdc_ln2",            tdc_ln2, 50);
	ShowConstant("qd_ln2",            qd_ln2, 65);
	ShowConstant("qdc_ln2",           qdc_ln2, 65);

	// --- sqrt(2) ---
	std::cout << "\n  --- sqrt(2) ---\n";
	ShowConstant("std::numbers::sqrt2", std::numbers::sqrt2, 17);
	ShowConstant("dd_sqrt2",            dd_sqrt2, 33);
	ShowConstant("ddc_sqrt2",           ddc_sqrt2, 33);
	ShowConstant("tdc_sqrt2",           tdc_sqrt2, 50);
	ShowConstant("qd_sqrt2",           qd_sqrt2, 65);
	ShowConstant("qdc_sqrt2",          qdc_sqrt2, 65);

	// --- phi (golden ratio) ---
	std::cout << "\n  --- phi (golden ratio) ---\n";
	ShowConstant("std::numbers::phi",  std::numbers::phi, 17);
	ShowConstant("dd_phi",             dd_phi, 33);
	ShowConstant("ddc_phi",            ddc_phi, 33);
	ShowConstant("tdc_phi",            tdc_phi, 50);
	ShowConstant("qd_phi",            qd_phi, 65);
	ShowConstant("qdc_phi",           qdc_phi, 65);

	// =================================================================
	//  Part 3 — When the extra digits matter
	// =================================================================

	Separator("PART 3:  When extra digits matter — computing sin(pi)");

	std::cout << "sin(pi) is exactly 0.  But the precision of your pi constant\n"
	          << "determines how close to 0 you actually get.  Each level of\n"
	          << "precision pushes the residual further toward zero.\n\n";

	// sin(pi) using std::numbers::pi
	double sin_double = std::sin(std::numbers::pi);
	std::cout << "  " << std::setw(24) << std::left << "double"
	          << "sin(pi) = " << std::scientific << std::setprecision(6) << sin_double
	          << "   (residual: " << std::abs(sin_double) << ")\n";

	long double sin_ldouble = std::sin(std::numbers::pi_v<long double>);
	std::cout << "  " << std::setw(24) << std::left << "long double"
	          << "sin(pi) = " << std::scientific << std::setprecision(6) << double(sin_ldouble)
	          << "   (residual: " << std::abs(double(sin_ldouble)) << ")\n";

	// sin(pi) using dd and qd
	dd sin_dd = sin(dd_pi);
	std::cout << "  " << std::setw(24) << std::left << "double-double"
	          << "sin(pi) = " << std::scientific << std::setprecision(6) << double(sin_dd)
	          << "   (residual: " << std::abs(double(sin_dd)) << ")\n";

	qd sin_qd = sin(qd_pi);
	std::cout << "  " << std::setw(24) << std::left << "quad-double"
	          << "sin(pi) = " << std::scientific << std::setprecision(6) << double(sin_qd)
	          << "   (residual: " << std::abs(double(sin_qd)) << ")\n";

	std::cout << std::defaultfloat;

	// =================================================================
	//  Part 4 — Area of a circle: precision accumulation
	// =================================================================

	Separator("PART 4:  Precision in practice — area of a unit circle");

	std::cout << "The area of a unit circle is exactly pi.  We compute\n"
	          << "4 * atan(1) and compare to the stored constant to see\n"
	          << "how many digits of agreement each precision level achieves.\n\n";

	// double
	double area_d = 4.0 * std::atan(1.0);
	double err_d = std::abs(area_d - std::numbers::pi);
	std::cout << "  " << std::setw(24) << std::left << "double"
	          << "4*atan(1) = " << std::setprecision(17) << area_d
	          << "   error = " << std::scientific << std::setprecision(2) << err_d << '\n';

	// dd
	dd area_dd = dd(4.0) * atan(dd(1.0));
	dd err_dd = abs(area_dd - dd_pi);
	std::cout << "  " << std::setw(24) << std::left << "double-double"
	          << "4*atan(1) = " << std::setprecision(33) << area_dd
	          << "   error = " << std::scientific << std::setprecision(2) << double(err_dd) << '\n';

	// qd
	qd area_qd = qd(4.0) * atan(qd(1.0));
	qd err_qd = abs(area_qd - qd_pi);
	std::cout << "  " << std::setw(24) << std::left << "quad-double"
	          << "4*atan(1) = " << std::setprecision(65) << area_qd
	          << "   error = " << std::scientific << std::setprecision(2) << double(err_qd) << '\n';

	std::cout << std::defaultfloat;

	// =================================================================
	//  Summary
	// =================================================================

	Separator("SUMMARY");

	std::cout << "  C++20 std::numbers    Great for everyday code.  Use it.\n"
	          << "                        Replaces non-standard M_PI, M_E, etc.\n"
	          << "                        Ceiling: long double (~18.5 digits on x86).\n\n"
	          << "  Universal dd/qd       For when double isn't enough:\n"
	          << "                        scientific computing, reference solutions,\n"
	          << "                        sensitivity analysis, compensated summation.\n"
	          << "                        dd: ~31 digits,  qd: ~62 digits.\n\n"
	          << "  Universal cascades    Same precision as dd/qd but using the\n"
	          << "                        floatcascade<N> template — a uniform\n"
	          << "                        interface for 2, 3, or 4 component\n"
	          << "                        expansions.\n\n"
	          << "  Rule of thumb:        Use std::numbers::pi for double.\n"
	          << "                        Use dd_pi / qd_pi when you need more.\n"
	          << "                        Never use M_PI — it is not standard C++.\n";

	std::cout << std::endl;
	return EXIT_SUCCESS;
}
catch (char const* msg) {
	std::cerr << "Caught ad-hoc exception: " << msg << std::endl;
	return EXIT_FAILURE;
}
catch (const sw::universal::universal_arithmetic_exception& err) {
	std::cerr << "Caught unexpected universal arithmetic exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (std::runtime_error& err) {
	std::cerr << "Caught unexpected runtime error: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (...) {
	std::cerr << "Caught unknown exception" << std::endl;
	return EXIT_FAILURE;
}
