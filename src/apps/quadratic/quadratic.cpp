/** ************************************************************************
* quadratic.cpp: a pedagogical exploration of catastrophic cancellation
*
* The quadratic formula is the first equation every student learns to
* solve analytically, yet it hides a numerical trap that catches even
* experienced engineers: catastrophic cancellation.
*
* This program tells a story in four acts:
*
*   Act I   — "Just use float"
*             A textbook equation gives a root that is completely wrong.
*
*   Act II  — "Just use double"
*             A slightly harder equation defeats double precision too.
*
*   Act III — The brute-force fix: double-double and quad-double
*             Throwing 128 or 256 bits at the problem buys correctness,
*             but only pushes the cliff further away.
*
*   Act IV  — The algebraic fix: Vieta's formula
*             A one-line change makes even 32-bit float give the right
*             answer.  Zero cost.  Zero extra bits.
*
* The moral: more bits is a band-aid; understanding the algorithm is
* the cure.  But when you cannot rearrange the algebra — and many
* real problems cannot — you need extended precision arithmetic,
* which is exactly what the Universal library provides.
*
* References:
*   Kahan, "On the Cost of Floating-Point Computation Without
*           Extra-Precise Arithmetic"
*   https://people.eecs.berkeley.edu/~wkahan/Qdrtcs.pdf
*   Goldberg, "What Every Computer Scientist Should Know About
*              Floating-Point Arithmetic" (1991)
*   Higham, "Accuracy and Stability of Numerical Algorithms" (2002)
*   https://pavpanchekha.com/blog/accurate-quadratic.html
*
* @author:     Theodore Omtzigt
* @date:       2023-01-04
* @copyright:  Copyright (c) 2023-2026 Stillwater Supercomputing, Inc.
* @license:    MIT Open Source license
*
* This file is part of the MPADAO project
* *************************************************************************
*/
#include <universal/utility/directives.hpp>
#include <cstdint>
#include <limits>
#include <iomanip>
#include <string>
#include <cmath>

// Universal number systems
#include <universal/number/cfloat/cfloat.hpp>
#include <universal/number/posit/posit.hpp>
#include <universal/number/dd/dd.hpp>
#include <universal/number/qd/qd.hpp>

// =====================================================================
//  Type aliases
// =====================================================================

using Float32  = sw::universal::cfloat<32, 8, uint32_t>;
using Float64  = sw::universal::cfloat<64, 11, uint32_t>;
using Posit32  = sw::universal::posit<32, 2>;
using Posit64  = sw::universal::posit<64, 2>;
using DD       = sw::universal::dd;    // double-double  (~31 decimal digits)
using QD       = sw::universal::qd;    // quad-double    (~62 decimal digits)

// =====================================================================
//  The naive quadratic formula
// =====================================================================
//
//  Given  a*x^2 + b*x + c = 0,  compute:
//
//      x = (-b +/- sqrt(b^2 - 4ac)) / (2a)
//
//  When b^2 >> 4ac the square root is very close to |b|, and the
//  root whose sign causes -b and sqrt to nearly cancel loses all
//  significant digits.  The number of decimal digits lost is
//  approximately log10(b^2 / 4ac).
//

template<typename Scalar>
std::pair<Scalar, Scalar> QuadraticNaive(const Scalar& a, const Scalar& b, const Scalar& c) {
	Scalar discriminant = b * b - Scalar(4) * a * c;
	Scalar sq = sqrt(discriminant);
	Scalar x1 = (-b - sq) / (Scalar(2) * a);
	Scalar x2 = (-b + sq) / (Scalar(2) * a);
	return { x1, x2 };
}

// =====================================================================
//  The stable quadratic formula  (Citardauq / Vieta's)
// =====================================================================
//
//  Compute the root whose terms REINFORCE (no cancellation),
//  then obtain the other root from Vieta's relation:
//
//      x1 * x2 = c / a
//
//  This works at every precision, including float.
//

template<typename Scalar>
std::pair<Scalar, Scalar> QuadraticStable(const Scalar& a, const Scalar& b, const Scalar& c) {
	Scalar discriminant = b * b - Scalar(4) * a * c;
	Scalar sq = sqrt(discriminant);

	// Choose the sign that adds magnitudes rather than cancels them
	Scalar x1;
	if (double(b) >= 0.0)
		x1 = (-b - sq) / (Scalar(2) * a);   // both terms negative
	else
		x1 = (-b + sq) / (Scalar(2) * a);   // both terms positive

	// Vieta's formula for the other root
	Scalar x2 = c / (a * x1);

	return { x1, x2 };
}

// =====================================================================
//  Helpers
// =====================================================================

constexpr int LBL = 22;   // label column width
constexpr int VAL = 22;   // value column width

template<typename Scalar>
void ShowRoots(const std::string& label, const Scalar& a, const Scalar& b, const Scalar& c,
               double true_x1, double true_x2) {
	auto [x1, x2] = QuadraticNaive(a, b, c);
	double v1 = double(x1), v2 = double(x2);
	double err2 = (true_x2 != 0.0) ? std::abs(v2 - true_x2) / std::abs(true_x2) : std::abs(v2);

	std::cout << "  " << std::setw(LBL) << std::left << label
	          << "x1 = " << std::setw(VAL) << std::right << v1
	          << "   x2 = " << std::setw(VAL) << v2;
	if (err2 > 0.01)
		std::cout << "   <-- x2 relative error: " << std::scientific << std::setprecision(1) << err2 << std::defaultfloat;
	std::cout << '\n';
}

template<typename Scalar>
void ShowStableRoots(const std::string& label, const Scalar& a, const Scalar& b, const Scalar& c,
                     double, double true_x2) {
	auto [x1, x2] = QuadraticStable(a, b, c);
	double v1 = double(x1), v2 = double(x2);
	double err2 = (true_x2 != 0.0) ? std::abs(v2 - true_x2) / std::abs(true_x2) : std::abs(v2);

	std::cout << "  " << std::setw(LBL) << std::left << label
	          << "x1 = " << std::setw(VAL) << std::right << v1
	          << "   x2 = " << std::setw(VAL) << v2;
	if (err2 < 1e-6)
		std::cout << "   <-- correct!";
	std::cout << '\n';
}

void Separator(const std::string& title) {
	std::cout << "\n" << std::string(70, '=') << "\n"
	          << title << "\n"
	          << std::string(70, '=') << "\n\n";
}

// =====================================================================
//  Show the bit-level anatomy of the cancellation
// =====================================================================

template<typename Real>
void ShowCancellation(const std::string& label, Real a, Real b, Real c) {
	Real b2    = b * b;
	Real fourac = Real(4) * a * c;
	Real disc  = b2 - fourac;
	Real sq    = sqrt(disc);
	Real neg_b = -b;
	Real numer = neg_b + sq;          // the cancellation happens here
	Real denom = Real(2) * a;
	Real root  = numer / denom;

	std::cout << "  Anatomy of the cancellation (" << label << "):\n";
	std::cout << "    b^2              = " << std::setprecision(17) << double(b2) << '\n';
	std::cout << "    4ac              = " << double(fourac) << '\n';
	std::cout << "    b^2 - 4ac        = " << double(disc) << '\n';
	std::cout << "    sqrt(b^2 - 4ac)  = " << double(sq) << '\n';
	std::cout << "    -b               = " << double(neg_b) << '\n';
	std::cout << "    -b + sqrt(...)   = " << double(numer) << "   <-- cancellation\n";
	std::cout << "    x2 = above / 2a  = " << double(root) << '\n';
	std::cout << std::setprecision(15);
}

// =====================================================================
//  main — the four-act story
// =====================================================================

int main()
try {
	using namespace sw::universal;

	std::cout << std::setprecision(15);

	// =================================================================
	//  Prologue
	// =================================================================

	Separator("CATASTROPHIC CANCELLATION IN THE QUADRATIC FORMULA");

	std::cout << "The quadratic formula  x = (-b +/- sqrt(b^2 - 4ac)) / 2a\n"
	          << "is the first closed-form solution every student learns.\n"
	          << "But when b^2 >> 4ac, the square root is almost |b|, and\n"
	          << "the subtraction  -b + sqrt(...)  destroys all the significant\n"
	          << "digits we need.  The number of digits lost is approximately\n"
	          << "log10(b^2 / 4ac).\n";

	// =================================================================
	//  Act I — Float fails
	// =================================================================

	Separator("ACT I:  \"Just use float\" — it gives a wrong answer");

	// Equation: x^2 + 100000*x + 1 = 0
	// True roots: x1 = -99999.99999..., x2 = -0.00001000000000010...
	// Digits lost: log10(1e10/4) = 9.4 — float has only 7.2

	double true_x1_case1 = -99999.99999;
	double true_x2_case1 = -1.0e-5;    // via Vieta's: c/(a*x1) = 1/99999.99999

	std::cout << "Equation:    x^2 + 100000*x + 1 = 0\n"
	          << "True roots:  x1 ~ -100000,  x2 ~ -0.00001\n"
	          << "Digits lost: log10(10^10 / 4) = 9.4\n"
	          << "Float has only 7.2 decimal digits of precision.\n\n";

	ShowRoots("float (naive)",  1.0f, 1.0e5f, 1.0f, true_x1_case1, true_x2_case1);
	ShowRoots("double (naive)", 1.0,  1.0e5,  1.0,  true_x1_case1, true_x2_case1);

	std::cout << "\n  Float gives x2 = 0.  Completely wrong.\n"
	          << "  Double gets x2 right — it has enough spare digits.\n"
	          << "  \"No problem, we'll just use double.\"\n";

	// Show the bit-level details
	std::cout << '\n';
	ShowCancellation("float", 1.0f, 1.0e5f, 1.0f);

	// =================================================================
	//  Act II — Double also fails
	// =================================================================

	Separator("ACT II:  \"Just use double\" — it fails too");

	// Equation: x^2 + 1000000000*x + 1 = 0
	// True roots: x1 = -999999999.999999999..., x2 = -1e-9
	// Digits lost: log10(1e18/4) = 17.4 — double has only 15.9

	double true_x1_case2 = -1.0e9;
	double true_x2_case2 = -1.0e-9;   // via Vieta's: 1 / 1e9

	std::cout << "Equation:    x^2 + 10^9 * x + 1 = 0\n"
	          << "True roots:  x1 ~ -10^9,  x2 ~ -10^-9\n"
	          << "Digits lost: log10(10^18 / 4) = 17.4\n"
	          << "Double has only 15.9 decimal digits of precision.\n\n";

	ShowRoots("float (naive)",  1.0f, 1.0e9f, 1.0f, true_x1_case2, true_x2_case2);
	ShowRoots("double (naive)", 1.0,  1.0e9,  1.0,  true_x1_case2, true_x2_case2);

	std::cout << "\n  Both float and double give x2 = 0.\n"
	          << "  The answer is off by 100%.  This is not a rounding error.\n"
	          << "  This is a qualitatively wrong answer.\n";

	// Show the bit-level details for double
	std::cout << '\n';
	ShowCancellation("double", 1.0, 1.0e9, 1.0);

	// =================================================================
	//  Act III — The brute-force fix: more bits
	// =================================================================

	Separator("ACT III:  The brute-force fix — double-double and quad-double");

	std::cout << "A double-double (dd) pairs two doubles to get ~31 decimal digits.\n"
	          << "A quad-double (qd) uses four doubles for ~62 decimal digits.\n"
	          << "With 31 digits, we can afford to lose 17 and still have 14 left.\n\n";

	// Case 1 with dd/qd
	std::cout << "Case 1: x^2 + 10^5 * x + 1 = 0   (x2 ~ -10^-5)\n";
	ShowRoots("double-double", DD(1.0), DD(1.0e5),  DD(1.0), true_x1_case1, true_x2_case1);
	ShowRoots("quad-double",   QD(1.0), QD(1.0e5),  QD(1.0), true_x1_case1, true_x2_case1);

	// Case 2 with dd/qd
	std::cout << "\nCase 2: x^2 + 10^9 * x + 1 = 0   (x2 ~ -10^-9)\n";
	ShowRoots("double-double", DD(1.0), DD(1.0e9),  DD(1.0), true_x1_case2, true_x2_case2);
	ShowRoots("quad-double",   QD(1.0), QD(1.0e9),  QD(1.0), true_x1_case2, true_x2_case2);

	std::cout << "\n  Double-double and quad-double recover the small root.\n"
	          << "  But this only pushes the cliff further away.\n";

	// Show a case that defeats double-double
	// b = 1e17: digits lost = log10(1e34/4) = 33.6, exceeds dd's ~31 digits
	double true_x1_case3 = -1.0e17;
	double true_x2_case3 = -1.0e-17;

	std::cout << "\nCase 3: x^2 + 10^17 * x + 1 = 0   (x2 ~ -10^-17)\n";
	std::cout << "  Digits lost: log10(10^34 / 4) = 33.6 — exceeds dd's 31 digits.\n";
	ShowRoots("double-double", DD(1.0), DD(1.0e17), DD(1.0), true_x1_case3, true_x2_case3);
	ShowRoots("quad-double",   QD(1.0), QD(1.0e17), QD(1.0), true_x1_case3, true_x2_case3);

	std::cout << "\n  Double-double fails on Case 3, but quad-double (62 digits) survives.\n"
	          << "  The pattern is clear: every fixed precision has a breaking point.\n";

	// =================================================================
	//  Act IV — The algebraic fix: Vieta's formula
	// =================================================================

	Separator("ACT IV:  The algebraic fix — Vieta's formula");

	std::cout << "Instead of computing both roots with the naive formula, compute\n"
	          << "the root whose terms REINFORCE (no cancellation), then use\n"
	          << "Vieta's relation  x1 * x2 = c/a  to get the other root.\n\n"
	          << "This costs nothing in performance and works at EVERY precision,\n"
	          << "including plain 32-bit float.\n\n";

	// Case 1 — stable formula
	std::cout << "Case 1: x^2 + 10^5 * x + 1 = 0   (x2 ~ -10^-5)\n";
	ShowStableRoots("float (stable)",  1.0f, 1.0e5f, 1.0f, true_x1_case1, true_x2_case1);
	ShowStableRoots("double (stable)", 1.0,  1.0e5,  1.0,  true_x1_case1, true_x2_case1);

	// Case 2 — stable formula
	std::cout << "\nCase 2: x^2 + 10^9 * x + 1 = 0   (x2 ~ -10^-9)\n";
	ShowStableRoots("float (stable)",  1.0f, 1.0e9f, 1.0f, true_x1_case2, true_x2_case2);
	ShowStableRoots("double (stable)", 1.0,  1.0e9,  1.0,  true_x1_case2, true_x2_case2);

	// Case 3 — stable formula
	std::cout << "\nCase 3: x^2 + 10^17 * x + 1 = 0   (x2 ~ -10^-17)\n";
	ShowStableRoots("double (stable)", 1.0,  1.0e17, 1.0,  true_x1_case3, true_x2_case3);

	std::cout << "\n  Even plain float gives the right answer with the stable formula.\n"
	          << "  No extra memory, no performance penalty, no wider types needed.\n";

	// =================================================================
	//  Act V — Comparison across Universal number types
	// =================================================================

	Separator("COMPARISON: the same equation across Universal number types");

	std::cout << "x^2 + 10^9 * x + 1 = 0  (the case that defeats double)\n\n"
	          << "  Naive formula:\n";
	ShowRoots("cfloat<32,8>",   Float32(1.0f), Float32(1.0e9f), Float32(1.0f), true_x1_case2, true_x2_case2);
	ShowRoots("cfloat<64,11>",  Float64(1.0),  Float64(1.0e9),  Float64(1.0),  true_x1_case2, true_x2_case2);
	ShowRoots("posit<32,2>",    Posit32(1.0f), Posit32(1.0e9f), Posit32(1.0f), true_x1_case2, true_x2_case2);
	ShowRoots("posit<64,2>",    Posit64(1.0),  Posit64(1.0e9),  Posit64(1.0),  true_x1_case2, true_x2_case2);
	ShowRoots("double-double",  DD(1.0),       DD(1.0e9),       DD(1.0),       true_x1_case2, true_x2_case2);
	ShowRoots("quad-double",    QD(1.0),       QD(1.0e9),       QD(1.0),       true_x1_case2, true_x2_case2);

	std::cout << "\n  Stable formula:\n";
	ShowStableRoots("cfloat<32,8>",   Float32(1.0f), Float32(1.0e9f), Float32(1.0f), true_x1_case2, true_x2_case2);
	ShowStableRoots("cfloat<64,11>",  Float64(1.0),  Float64(1.0e9),  Float64(1.0),  true_x1_case2, true_x2_case2);
	ShowStableRoots("posit<32,2>",    Posit32(1.0f), Posit32(1.0e9f), Posit32(1.0f), true_x1_case2, true_x2_case2);
	ShowStableRoots("posit<64,2>",    Posit64(1.0),  Posit64(1.0e9),  Posit64(1.0),  true_x1_case2, true_x2_case2);
	ShowStableRoots("double-double",  DD(1.0),       DD(1.0e9),       DD(1.0),       true_x1_case2, true_x2_case2);
	ShowStableRoots("quad-double",    QD(1.0),       QD(1.0e9),       QD(1.0),       true_x1_case2, true_x2_case2);

	// =================================================================
	//  Epilogue
	// =================================================================

	Separator("THE LESSON");

	std::cout << "1. Catastrophic cancellation is not a rounding error.\n"
	          << "   It is a structural defect in the algorithm that destroys\n"
	          << "   information before rounding even enters the picture.\n\n"
	          << "2. \"Just use double\" is not a solution.  For the quadratic\n"
	          << "   formula, any fixed precision has a breaking point:\n"
	          << "   pick b large enough and the small root vanishes.\n\n"
	          << "3. The algebraic fix (Vieta's formula) costs nothing and\n"
	          << "   works at every precision.  When a reformulation exists,\n"
	          << "   use it.\n\n"
	          << "4. Many real problems have no algebraic fix.  Iterative\n"
	          << "   solvers, neural network training, physical simulations —\n"
	          << "   these accumulate cancellations that cannot be rearranged\n"
	          << "   away.  For those problems, you need extended precision\n"
	          << "   arithmetic: double-double, quad-double, posits, or\n"
	          << "   adaptive types like ereal.\n\n"
	          << "5. The Universal library lets you explore this entire design\n"
	          << "   space — from 8-bit floats to quad-double — with a single\n"
	          << "   template parameter change.\n";

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
catch (const sw::universal::universal_internal_exception& err) {
	std::cerr << "Caught unexpected universal internal exception: " << err.what() << std::endl;
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
