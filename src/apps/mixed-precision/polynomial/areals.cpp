// areals.cpp: polynomial evaluation using areal number system
//
// Copyright (C) 2017-2023 Stillwater Supercomputing, Inc.
//
// This file is part of the MPADAO project, which is released under an MIT Open Source license.
#include <iostream>
#include <vector>
#include <cstdint>
#include <universal/number/areal/areal.hpp>

using Areal = sw::universal::areal<8, 2, uint8_t>;

Areal arealPolynomial(const std::vector<int>& coef, const Areal& x) {
	if (coef.size() < 2) {
		std::cerr << "Coefficient set is too small to represent a polynomial\n";
		return Areal(0);
	}

	// Horner's method: p(x) = coef[n-1]*x^(n-1) + ... + coef[1]*x + coef[0]
	// evaluated as: p(x) = (...((coef[n-1]*x + coef[n-2])*x + coef[n-3])*x + ... + coef[0])
	size_t n = coef.size();
	Areal v = Areal(coef[n - 1]);
	for (size_t i = n - 1; i > 0; --i) {
		v = v * x + Areal(coef[i - 1]);
	}
	return v;
}
