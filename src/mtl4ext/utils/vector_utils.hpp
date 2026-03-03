#pragma once
// vector_utils.hpp :  include file containing templated utilities to work with vectors and matrices
//
// Copyright (C) 2017-2019 Stillwater Supercomputing, Inc.
//
// This file is part of the HPRBLAS project, which is released under an MIT Open Source license.

#include <random>

namespace sw {
namespace hprblas {

// initialize a vector
template<typename Vector, typename Scalar>
void init(Vector& x, const Scalar& value) {
	for (size_t i = 0; i < x.size(); ++i) x[i] = value;
}

// generate random data vector
template<typename element_T>
void randomVectorFill(size_t n, std::vector<element_T>& vec) {
	std::random_device rd;
	std::mt19937 engine(rd());
	std::uniform_real_distribution<double> dist(0.0, 1.0);
	for (size_t i = 0; i < n; i++) {
		vec[i] = (element_T)dist(engine);
	}
}

// generate a vector of random permutations around 0.0
// contraction is a right shift of the random variable causing smaller fluctuations
template<typename element_T>
void randomVectorFillAroundZeroEPS(size_t n, std::vector<element_T>& vec, size_t contraction = 6) {
	std::random_device rd;
	std::mt19937 engine(rd());
	double scale = 1.0 / (1 << contraction);
	std::uniform_real_distribution<double> dist(-scale, scale);
	for (size_t i = 0; i < n; i++) {
		vec[i] = (element_T)dist(engine);
	}
}

// generate a vector of random permutations around 1.0
// contraction is a right shift of the random variable causing smaller fluctuations
template<typename element_T>
void randomVectorFillAroundOneEPS(size_t n, std::vector<element_T>& vec, size_t contraction = 6) {
	std::random_device rd;
	std::mt19937 engine(rd());
	double scale = 1.0 / (1 << contraction);
	std::uniform_real_distribution<double> dist(-scale, scale);
	for (size_t i = 0; i < n; i++) {
		vec[i] = (element_T)(1.0 + dist(engine));
	}
}

// print a sampling of the provided vector
// if samples is set to 0, all elements of the vector are printed
template<typename element_T>
void sampleVector(std::string vec_name, std::vector<element_T>& vec, uint32_t start = 0, uint32_t incr = 1, uint32_t nrSamples = 0) {
	std::cout << "Vector sample is: " << '\n';
	if (nrSamples) {
		uint32_t printed = 0;
		for (uint32_t i = start; i < vec.size(); i += incr) {
			if (printed < nrSamples) {
				printed++;
				std::cout << vec_name << "[" << std::setw(3) << i << "] = " << std::setprecision(15) << vec[i] << '\n';
			}
		}
	}
	else {
		for (uint32_t i = start; i < vec.size(); i += incr) {
			std::cout << vec_name << "[" << std::setw(3) << i << "] = " << std::setprecision(15) << vec[i] << '\n';
		}
	}
	std::cout << std::endl;
}

} // namespace hprblas
} // namespace sw
