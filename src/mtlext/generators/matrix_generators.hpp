#pragma once
// matrix_generators.hpp: generators for special matrices
//
// Copyright (C) 2017-2020 Stillwater Supercomputing, Inc.
//
// This file is part of the HPRBLAS project, which is released under an MIT Open Source license.
#include <cstdint>
#include <random>
#include <algorithm>
#include <mtl/mtl.hpp>
#include <universal/number/posit/posit.hpp>
#include <math/functions/binomial.hpp>

namespace sw {
namespace hprblas {

// fill a dense matrix with random values between [lowerbound, upperbound]
template <typename Matrix>
void uniform_rand(Matrix& A, double lowerbound = 0.0, double upperbound = 1.0)
{
	// Use random_device to generate a seed for Mersenne twister engine.
	std::random_device rd{};
	// Use Mersenne twister engine to generate pseudo-random numbers.
	std::mt19937 engine{ rd() };
	// "Filter" MT engine's output to generate pseudo-random double values,
	// **uniformly distributed** on the closed interval [lowerbound, upperbound].
	// (Note that the range is [inclusive, inclusive].)
	std::uniform_real_distribution<double> dist{ lowerbound, upperbound };

	using value_type = typename Matrix::value_type;
	using size_type  = typename Matrix::size_type;

	// generate and assign random values in A
	for (size_type r = 0; r < A.num_rows(); r++) {
		for (size_type c = 0; c < A.num_cols(); c++) {
			A(r, c) = value_type(dist(engine));
		}
	}
}

// create a dense matrix with random values between [lowerbound, upperbound]
template<typename Scalar>
mtl::mat::dense2D<Scalar> uniform_rand(size_t m, size_t n, double lowerbound = 0.0, double upperbound = 1.0) {
	mtl::mat::dense2D<Scalar> A(m, n);
	uniform_rand(A, lowerbound, upperbound);
	return A;
}

// fill a dense MTL matrix with random values between [lowerbound, upperbound]
template <typename Matrix>
void uniform_rand_sorted(Matrix& A, double lowerbound = 0.0, double upperbound = 1.0)
{
	// Use random_device to generate a seed for Mersenne twister engine.
	std::random_device rd{};
	// Use Mersenne twister engine to generate pseudo-random numbers.
	std::mt19937 engine{ rd() };
	// "Filter" MT engine's output to generate pseudo-random double values,
	// **uniformly distributed** on the closed interval [lowerbound, upperbound].
	// (Note that the range is [inclusive, inclusive].)
	std::uniform_real_distribution<double> dist{ lowerbound, upperbound };

	using value_type = typename Matrix::value_type;
	using size_type  = typename Matrix::size_type;

	// generate a good set of randoms
	std::vector<value_type> v(A.size());
	for (size_type r = 0; r < A.num_rows(); ++r) {
		for (size_type c = 0; c < A.num_cols(); ++c) {
			v.push_back(value_type(dist(engine)));
		}
	}
	// sort them so that we have better control over the scale of each element in a row vector
	sort(v.begin(), v.end(), std::greater<value_type>());

	// for each row minus the last column, calculate the sum of elements without rounding
	sw::universal::posit<value_type::nbits, value_type::es> one(1), p;
	for (size_type r = 0; r < A.num_rows(); ++r) {
		sw::universal::quire<value_type::nbits, value_type::es> q1, q2;
		size_type lastElement = A.num_cols() - 1;
		for (size_type c = 0; c < lastElement; ++c) {
			q1 += sw::universal::quire_mul(one, v[r*A.num_cols() + c]);
		}
		// truncate the value in the quire
		convert(q1.to_value(), p);
		// calculate the difference between the truncated and the non-truncated quire values
		q2 = p;
		q2 -= q1;
		convert(q2.to_value(), p);
		v[r*A.num_cols() + lastElement] = p;
	}

	// assign sorted values in A
	size_type i = 0;
	for (size_type r = 0; r < A.num_rows(); r++) {
		for (size_type c = 0; c < A.num_cols(); c++) {
			A(r, c) = v[i++];
		}
	}
}

// fill a dense MTL matrix with diagonally dominant random values between [lowerbound, upperbound]
template<typename Matrix>
void uniform_rand_diagonally_dominant(Matrix& A, double lowerbound = 0.0, double upperbound = 1.0) {
	// generate off-diagonal entries, calculate the sum, scale to upperbound, and set diagonal entry to slightly larger

	// Use random_device to generate a seed for Mersenne twister engine.
	std::random_device rd{};
	// Use Mersenne twister engine to generate pseudo-random numbers.
	std::mt19937 engine{ rd() };
	// "Filter" MT engine's output to generate pseudo-random double values,
	// **uniformly distributed** on the closed interval [lowerbound, upperbound].
	// (Note that the range is [inclusive, inclusive].)
	std::uniform_real_distribution<double> dist{ lowerbound, upperbound };

	using value_type = typename Matrix::value_type;
	using size_type  = typename Matrix::size_type;

	// no need to null A as each element is assigned explicitly
	for (size_type r = 0; r < A.num_rows(); ++r) {
		// generate a random vector of N
		size_type N = A.num_cols();
		mtl::vec::dense_vector<value_type> v(N);
		for (size_type c = 0; c < N; ++c) {
			v[c] = value_type(dist(engine));
		}
		// add the sum of the other row elements to the diagonal
		for (size_type c = 0; c < N; ++c) {
			if (r != c) v[r] += v[c];
		}
		value_type factor = v[r] / upperbound;
		for (size_type c = 0; c < N; ++c) {
			A(r, c) = v[c] / factor;
		}
	}
}

// Random Orthogonal Matrices

/*
Standard methods for generating random orthogonal matrices with Haar distribution
are based on the method of Heiberger (1978). With this method, an (n x n) matrix A is
first generated with entries xij ~ Normal(0; 1). Then a QR factorization (X = QR) is computed.
This method provides a random Q with correct distribution.
*/
template<typename Matrix>
void uniform_random_orthogonal_Heiberger(Matrix& Q) {
	// Use random_device to generate a seed for Mersenne twister engine.
	std::random_device rd{};
	// Use Mersenne twister engine to generate pseudo-random numbers.
	std::mt19937 engine{ rd() };
	// "Filter" MT engine's output to generate pseudo-random double values,
	// **uniformly distributed** on the closed interval [lowerbound, upperbound].
	// (Note that the range is [inclusive, inclusive].)
	std::uniform_real_distribution<double> dist{ 0.0, 1.0 };

	using value_type = typename Matrix::value_type;
	using size_type  = typename Matrix::size_type;
	Matrix A(mtl::mat::num_rows(Q), mtl::mat::num_cols(Q));

	// fill A with elements from Normal(0,1)
	for (size_type r = 0; r < A.num_rows(); r++) {
		for (size_type c = 0; c < A.num_cols(); c++) {
			A(r, c) = value_type(dist(engine));
		}
	}
	// QR factorization to generate orthonormal matrix Q
	mtl::vec::dense_vector<value_type> tau;
	mtl::qr_factor(A, tau);
	Q = mtl::qr_extract_Q(A, tau);
	std::cout << A << std::endl;
	std::cout << Q << std::endl;
}

/*
C.3: Generating a Random Matrix with Specified Eigenvalues
Generate random orthogonal matrix G. W. Stewart (1980).

start RandOrthog(n);
A = I(n); // identity matrix
d = j(n, 1, 0);
d[n] = sgn(RndNormal(1, 1)); // +/- 1
do k = n - 1 to 1 by - 1;
	// generate random Householder transformation
	x = RndNormal(n - k + 1, 1); // column vector from N(0,1)
	s = sqrt(x[##]); // norm(x)
	sgn = sgn(x[1]);
	s = sgn*s;
	d[k] = -sgn;
	x[1] = x[1] + s;
	beta = s*x[1];
	// apply the Householder transformation to A
	y = x`*A[k:n, ];
	A[k:n, ] = A[k:n, ] - x*(y / beta);
end;
A = d # A; // change signs of i_th row when d[i]=-1
return(A);
finish;

// helper functions
// return matrix of same size as A with
// m[i,j]= {  1 if A[i,j]>=0
//         { -1 if A[i,j]< 0
// Similar to the SIGN function, except SIGN(0)=0
start sgn(A);
	return(choose(A >= 0, 1, -1));
finish;

// return (r x c) matrix of standard normal variates
start RndNormal(r, c);
	x = j(r, c);
	call randgen(x, "Normal");
	return(x);
finish;

// The following statements call the RANDORTHOG function to generate a random 4x4 orthogonal matrix.
call randseed(1);
Q = RandOrthog(4);
print(Q);
*/
template<typename Matrix>
void uniform_rand_orthogonal(Matrix& A) {
	throw std::runtime_error("uniform_rand_orthogonal: not yet implemented");
}

//
// fill a dense upper-triangular matrix with elements: (i,j) = 0.5 + (1 + j - i)*0.5
template <typename Matrix>
void fill_U(Matrix& A, double lowerbound = 0.0, double upperbound = 1.0)
{
	using value_type = typename Matrix::value_type;
	using size_type  = typename Matrix::size_type;

	// set all elements to zero first
	mtl::set_to_zero(A);

	// fill upper-triangular elements
	for (size_type r = 0; r < A.num_rows(); r++) {
		for (size_type c = r; c < A.num_cols(); c++) {
			A(r, c) = value_type(0.5) + value_type(1 + c - r)*value_type(0.5);
		}
	}
}

// fill a dense lower-triangular matrix with elements: (i,j) = 0.5 + (1 + i - j)*0.5
template <typename Matrix>
void fill_L(Matrix& A, double lowerbound = 0.0, double upperbound = 1.0)
{
	using value_type = typename Matrix::value_type;
	using size_type  = typename Matrix::size_type;

	// set all elements to zero first
	mtl::set_to_zero(A);

	// fill lower-triangular elements
	for (size_type r = 0; r < A.num_rows(); r++) {
		for (size_type c = 0; c <= r; c++) {
			A(r, c) = value_type(0.5) + value_type(1 + r - c)*value_type(0.5);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
/// Hilbert matrices

// Greatest Common Divisor of two numbers, a and b
template<typename IntegerType>
IntegerType gcd(IntegerType a, IntegerType b) {
	return b == 0 ? a : sw::hprblas::gcd(b, a % b);
}

// Least Common Multiple of n numbers
template<typename IntegerType>
IntegerType findlcm(const std::vector<IntegerType>& v) {
	IntegerType lcm = v[0];
	for (size_t i = 1; i < v.size(); i++) {
		lcm = (v[i] * lcm) / sw::hprblas::gcd(v[i], lcm);
	}
	return lcm;
}

// Generate the scaling factor of a Hilbert matrix so that its elements are representable
// that is, no infinite expensions of rationals, such as 1/3, 1/10, etc.
template<typename IntegerType>
IntegerType HilbertScalingFactor(IntegerType N) {
	std::vector<IntegerType> coef;
	for (IntegerType i = 2; i <= N; ++i) coef.push_back(i);
	for (IntegerType j = 2; j <= N; ++j) coef.push_back(N + j - IntegerType(1));
	return findlcm(coef);
}

// Generate a scaled/unscaled Hilbert matrix depending on the bScale parameter
template<typename Scalar>
size_t GenerateHilbertMatrix(mtl::mat::dense2D<Scalar>& M, bool bScale = true) {
	assert(M.num_rows() == M.num_cols());
	size_t N = M.num_rows();
	size_t lcm = HilbertScalingFactor(N); // always calculate the Least Common Multiplier
	Scalar scale = bScale ? Scalar(lcm) : Scalar(1);
	for (int i = 1; i <= static_cast<int>(N); ++i) {
		for (int j = 1; j <= static_cast<int>(N); ++j) {
			M(i - 1, j - 1) = scale / Scalar(i + j - 1);
		}
	}
	return lcm;
}

template<typename Scalar>
void GenerateHilbertMatrixInverse(mtl::mat::dense2D<Scalar>& m, Scalar scale = Scalar(1.0)) {
	assert(m.num_rows() == m.num_cols());
	size_t N = m.num_rows();
	for (int i = 1; i <= static_cast<int>(N); ++i) {
		for (int j = 1; j <= static_cast<int>(N); ++j) {
			Scalar sign = ((i + j) % 2) ? Scalar(-1) : Scalar(1);
			Scalar factor1 = Scalar(i + j - 1);
			Scalar factor2 = Scalar(sw::math::function::binomial<uint64_t>(N + i - 1, N - j));
			Scalar factor3 = Scalar(sw::math::function::binomial<uint64_t>(N + j - 1, N - i));
			Scalar factor4 = Scalar(sw::math::function::binomial<uint64_t>(i + j - 2, i - 1));
			m(i - 1, j - 1) = Scalar(sign * factor1 * factor2 * factor3 * factor4 * factor4);
		}
	}
}

// isEqual compares two matrices
template<typename Matrix>
bool isEqual(const Matrix& lhs, const Matrix& rhs) {
	size_t r = lhs.num_rows();
	size_t c = lhs.num_cols();
	for (size_t i = 0; i < r; ++i) {
		for (size_t j = 0; j < c; ++j) {
			if (lhs(i, j) != rhs(i, j)) return false;
		}
	}
	return true;
}

} // namespace hprblas
} // namespace sw
