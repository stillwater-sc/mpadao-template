/** ************************************************************************
* gauss_jordan : example program comparing float vs posit matrix inversion algorithms
*
* @author:     Theodore Omtzigt
* @date:       2023-01-04
* @copyright:  Copyright (c) 2023 Stillwater Supercomputing, Inc.
* @license:    MIT Open Source license
*
* This file is part of the MPADAO project
* *************************************************************************
*/

/**
* NUMBER TYPES AND DENSE LINEAR ALGEBRA CONTAINERS
*
* Dense linear algebra libraries like MTL5 store matrix and vector elements
* in contiguous memory. This design is essential for performance: modern
* CPUs fetch cache lines (typically 64 bytes), and sequential access to
* contiguous elements maximizes cache hits, enables hardware prefetching,
* and allows SIMD vectorization.
*
* This works beautifully for FIXED-SIZE number types — float (4 bytes),
* double (8 bytes), posit<32,2> (4 bytes), cfloat<32,8> (4 bytes) — where
* every element occupies the same number of bytes and the entire element
* value lives inline in the container's memory block.
*
* VARIABLE-SIZE number types, such as boost::multiprecision::cpp_bin_float,
* present a fundamentally different challenge. These types manage their
* mantissa on the heap via internal pointers. When stored in a dense2D<quad>,
* the matrix memory block holds N*M quad *objects* contiguously, but each
* object contains a pointer to separately allocated heap storage. Traversing
* elements now causes pointer chasing — each element access follows an
* indirection to a different heap location, defeating spatial locality and
* hardware prefetching. The result is dramatically higher cache miss rates.
*
* This distinction also surfaces at compile time: fixed-size types are
* "literal types" in C++ — they can be constructed in constexpr contexts.
* MTL5's algebraic identity system (math::zero<T>(), math::one<T>()) is
* correctly designed as constexpr, which means it cannot be instantiated
* for non-literal types like cpp_bin_float_quad.
*
* In this example, we demonstrate both classes of types:
*  - float and posit<32,2>: fixed-size, literal, cache-friendly —
*    they use MTL5's operator* and operator- directly.
*  - cpp_bin_float_quad: variable-size, non-literal, cache-unfriendly —
*    requires explicit multiply/subtract loops that avoid math::zero<T>().
*
* For production use of extended precision in dense linear algebra, prefer
* fixed-size types from Universal (cfloat, posit, fixpnt) over Boost
* multiprecision. They provide the precision you need while remaining
* compatible with constexpr identities, cache-friendly layouts, and the
* full MTL5 operator algebra.
*/

#include <boost/multiprecision/cpp_bin_float.hpp>

#include <mtl/mtl.hpp>
#include <mtl_extensions.hpp>

// arithmetic types under study
#include <universal/number/cfloat/cfloat.hpp>
#include <universal/number/posit/posit.hpp>
#include <norms.hpp>
// matrix generators
#include <generators/matrix_generators.hpp>
#include <utils/print_utils.hpp>

namespace sw {
	namespace hprblas {

		template<typename Matrix>
		Matrix GaussJordanInversion(const Matrix& A) {
			using Scalar = typename Matrix::value_type;

			size_t m = A.num_rows();
			size_t n = A.num_cols();
			Matrix a(n, m), inv(m, n);
			// deep copy A into a
			for (size_t i = 0; i < m; ++i)
				for (size_t j = 0; j < n; ++j)
					a(i, j) = A(i, j);
			// initialize inv to identity
			for (size_t i = 0; i < m; ++i)
				for (size_t j = 0; j < n; ++j)
					inv(i, j) = (i == j) ? Scalar(1) : Scalar(0);

			// Performing elementary operations
			for (unsigned i = 0; i < m; ++i) {
				if (a(i, i) == 0) {
					unsigned c = 1;
					while (a(i + c, i) == 0 && (i + c) < n)	++c;
					if ((i + c) == n) break;

					for (unsigned k = 0; k < n; ++k) {
						std::swap(a(i, k), a(i + c, k));
						std::swap(inv(i, k), inv(i + c, k));
					}
				}
				// transform to diagonal matrix
				for (unsigned j = 0; j < m; j++) {
					if (i != j) {
						Scalar scale = a(j, i) / a(i, i);
						for (unsigned k = 0; k < n; ++k) {
							a(j, k) = a(j, k) - a(i, k) * scale;
							inv(j, k) = inv(j, k) - inv(i, k) * scale;
						}
					}
				}
			}
			// transform to identity matrix
			for (unsigned i = 0; i < m; ++i) {
				Scalar normalize = a(i, i);
				a(i, i) = Scalar(1);
				for (unsigned j = 0; j < n; ++j) {
					inv(i, j) /= normalize;
				}
			}
			return inv;
		}
	} // namespace hprblas
} // namespace sw

// ── Explicit dense multiply/subtract for non-literal types ─────────────
//
// MTL5's operator* and operator- use constexpr math::zero<T>() internally,
// which requires T to be a literal type. Boost multiprecision types are
// not literal (they heap-allocate their mantissa), so we provide explicit
// element-wise operations that use T{0} instead.
//
// For fixed-size, literal types (float, double, posit, cfloat), prefer
// MTL5's built-in operators — they are optimized and fully constexpr.

template<typename Scalar>
mtl::mat::dense2D<Scalar> mat_mul(const mtl::mat::dense2D<Scalar>& A,
                                   const mtl::mat::dense2D<Scalar>& B) {
	assert(A.num_cols() == B.num_rows());
	auto m = A.num_rows();
	auto n = B.num_cols();
	auto k = A.num_cols();
	mtl::mat::dense2D<Scalar> C(m, n);
	for (decltype(m) r = 0; r < m; ++r) {
		for (decltype(n) c = 0; c < n; ++c) {
			Scalar acc{0};
			for (decltype(k) i = 0; i < k; ++i) {
				acc += A(r, i) * B(i, c);
			}
			C(r, c) = acc;
		}
	}
	return C;
}

template<typename Scalar>
mtl::vec::dense_vector<Scalar> mat_vec_mul(const mtl::mat::dense2D<Scalar>& A,
                                            const mtl::vec::dense_vector<Scalar>& x) {
	assert(A.num_cols() == x.size());
	auto m = A.num_rows();
	auto n = A.num_cols();
	mtl::vec::dense_vector<Scalar> y(m);
	for (decltype(m) r = 0; r < m; ++r) {
		Scalar acc{0};
		for (decltype(n) c = 0; c < n; ++c) {
			acc += A(r, c) * x[c];
		}
		y[r] = acc;
	}
	return y;
}

template<typename Scalar>
mtl::vec::dense_vector<Scalar> vec_sub(const mtl::vec::dense_vector<Scalar>& a,
                                        const mtl::vec::dense_vector<Scalar>& b) {
	assert(a.size() == b.size());
	auto n = a.size();
	mtl::vec::dense_vector<Scalar> r(n);
	for (decltype(n) i = 0; i < n; ++i) {
		r[i] = a[i] - b[i];
	}
	return r;
}

// ── Test case for non-literal types (e.g. Boost multiprecision) ────────
//
// Variable-size types like cpp_bin_float_quad cannot use MTL5's constexpr
// identity system, so this specialization uses explicit element-wise
// operations. See the file header comment for the full explanation of
// why variable-size types require special treatment in dense containers.

template<typename Scalar>
void GenerateNumericalAnalysisTestCase_nonliteral(const std::string& header, unsigned N, bool verbose = false) {
	using namespace std;
	using namespace sw::hprblas;

	std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n" << header << std::endl;
	using Vector = mtl::vec::dense_vector<Scalar>;
	using Matrix = mtl::mat::dense2D<Scalar>;

	Matrix H(N, N);
	GenerateHilbertMatrix(H, false);
	Matrix Hinv = GaussJordanInversion(H);
	Matrix Href(N, N);
	GenerateHilbertMatrixInverse(Href);
	Matrix I(N, N);  // H * Hinv should yield the identity matrix

	// Use explicit multiply — MTL5's operator* requires constexpr zero<T>()
	// which is not available for non-literal types like cpp_bin_float_quad
	I = mat_mul(H, Hinv);

	if (verbose) {
		printMatrix(cout, "Hilbert matrix order 5", H);
		printMatrix(cout, "Hilbert inverse", Hinv);
		printMatrix(cout, "Hilbert inverse reference", Href);
		printMatrix(cout, "H * H^-1 => I", I);
	}

	// calculate the numerical error caused by the linear algebra computation
	Vector e(N), eprime(N), eabsolute(N);
	for (unsigned i = 0; i < N; ++i) e[i] = Scalar(1);
	eprime = mat_vec_mul(I, e);
	printVector(cout, "reference vector", e);
	printVector(cout, "error vector", eprime);
	// absolute error
	eabsolute = vec_sub(e, eprime);
	printVector(cout, "absolute error vector", eabsolute);
	cout << "L1 norm   " << l1_norm(eabsolute) << endl;
	cout << "L2 norm   " << l2_norm(eabsolute) << endl;
	cout << "Linf norm " << linf_norm(eabsolute) << endl;

	// relative error
	cout << "relative error\n";
	Scalar relative_error;
	relative_error = l1_norm(eabsolute) / l1_norm(e);
	cout << "L1 norm   " << relative_error << endl;
	relative_error = l2_norm(eabsolute) / l2_norm(e);
	cout << "L2 norm   " << relative_error << endl;
	relative_error = linf_norm(eabsolute) / linf_norm(e);
	cout << "Linf norm " << relative_error << endl;

	// error volume
	cout << "error bounding box volume\n";
	cout << "Measured in Euclidean distance    : " << error_volume(linf_norm(eabsolute), N, false) << endl;
	cout << "Measured in ULPs                  : " << error_volume(linf_norm(eabsolute), N, true) << " ulps^" << N << endl;
	Scalar ulp = numeric_limits<Scalar>::epsilon();
	cout << "L-infinitiy norm measured in ULPs : " << linf_norm(eabsolute) / ulp << " ulps" << endl;
}

// ── Test case for fixed-size, literal types ────────────────────────────
//
// Fixed-size types like float, double, and posit<32,2> are literal types:
// their values live entirely inline, they support constexpr construction,
// and they produce cache-friendly layouts in dense containers. MTL5's full
// operator algebra (operator*, operator-, etc.) works directly.

template<typename Scalar>
void GenerateNumericalAnalysisTestCase(const std::string& header, unsigned N, bool verbose = false) {
	using namespace std;
	using namespace sw::universal;
	using namespace sw::hprblas;

	std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n" << header << std::endl;
	using Vector = mtl::vec::dense_vector<Scalar>;
	using Matrix = mtl::mat::dense2D<Scalar>;

	Matrix H(N, N);
	GenerateHilbertMatrix(H, false);
	Matrix Hinv = GaussJordanInversion(H);
	Matrix Href(N, N);
	GenerateHilbertMatrixInverse(Href);
	Matrix I(N, N);  // H * Hinv should yield the identity matrix

	// MTL5's operator* works directly for literal types
	I = H * Hinv;

	if (verbose) {
		printMatrix(cout, "Hilbert matrix order 5", H);
		printMatrix(cout, "Hilbert inverse", Hinv);
		printMatrix(cout, "Hilbert inverse reference", Href);
		printMatrix(cout, "H * H^-1 => I", I);
	}

	// calculate the numerical error caused by the linear algebra computation
	Vector e(N), eprime(N), eabsolute(N);
	for (unsigned i = 0; i < N; ++i) e[i] = Scalar(1);
	// TODO: it is not clear that for posits this would be a fused matrix-vector operation
	eprime = I * e;
	printVector(cout, "reference vector", e);
	printVector(cout, "error vector", eprime);
	// absolute error
	eabsolute = e - eprime;
	printVector(cout, "absolute error vector", eabsolute);
	cout << "L1 norm   " << hex_format(l1_norm(eabsolute)) << "  " << l1_norm(eabsolute) << endl;
	cout << "L2 norm   " << hex_format(l2_norm(eabsolute)) << "  " << l2_norm(eabsolute) << endl;
	cout << "Linf norm " << hex_format(linf_norm(eabsolute)) << "  " << linf_norm(eabsolute) << endl;

	// relative error
	cout << "relative error\n";
	Scalar relative_error;
	relative_error = l1_norm(eabsolute) / l1_norm(e);
	cout << "L1 norm   " << hex_format(relative_error) << "  " << relative_error << endl;
	relative_error = l2_norm(eabsolute) / l2_norm(e);
	cout << "L2 norm   " << hex_format(relative_error) << "  " << relative_error << endl;
	relative_error = linf_norm(eabsolute) / linf_norm(e);
	cout << "Linf norm " << hex_format(relative_error) << "  " << relative_error << endl;

	// error volume
	cout << "error bounding box volume\n";
	cout << "Measured in Euclidean distance    : " << error_volume(linf_norm(eabsolute), N, false) << endl;
	cout << "Measured in ULPs                  : " << error_volume(linf_norm(eabsolute), N, true) << " ulps^" << N << endl;
	Scalar ulp = numeric_limits<Scalar>::epsilon();
	cout << "L-infinitiy norm measured in ULPs : " << linf_norm(eabsolute) / ulp << " ulps" << endl;
}

int main(int argc, char** argv)
try {
	using namespace std;
	using namespace sw::universal;

	using quad = boost::multiprecision::cpp_bin_float_quad;

	unsigned N = 5;
	bool Verbose = true;

	// Non-literal type: uses explicit element-wise operations
	// (see file header for why variable-size types need special treatment)
	GenerateNumericalAnalysisTestCase_nonliteral< quad >("IEEE quad precision (non-literal type)", N, Verbose);
	cout << "***********************************************************************************************\n";

	// Fixed-size literal types: use MTL5's built-in operator algebra directly
	GenerateNumericalAnalysisTestCase< posit<32, 2> >("posit<32,2>", N, Verbose);
	cout << "***********************************************************************************************\n";
	GenerateNumericalAnalysisTestCase< float >("IEEE single precision", N, Verbose);
	cout << "***********************************************************************************************\n";

	return EXIT_SUCCESS;
}
catch (char const* msg) {
	std::cerr << msg << std::endl;
	return EXIT_FAILURE;
}
catch (const sw::universal::posit_arithmetic_exception& err) {
	std::cerr << "Uncaught posit arithmetic exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const sw::universal::quire_exception& err) {
	std::cerr << "Uncaught quire exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const sw::universal::posit_internal_exception& err) {
	std::cerr << "Uncaught posit internal exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (std::runtime_error& err) {
	std::cerr << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (...) {
	std::cerr << "Caught unknown exception" << std::endl;
	return EXIT_FAILURE;
}
