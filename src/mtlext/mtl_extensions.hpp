#pragma once
// mtl_extensions.hpp :  include file containing templated utilities to work with matrices
//
// Copyright (C) 2017-2019 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.
#include <cstdint>
#include <random>
#include <algorithm>

namespace mtl {
namespace mat {

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
	for (size_type r = 0; r < num_rows(A); r++) {
		for (size_type c = 0; c < num_cols(A); c++) {
			A(r, c) = value_type(dist(engine));
		}
	}
}

} // namespace mat
} // namespace mtl
