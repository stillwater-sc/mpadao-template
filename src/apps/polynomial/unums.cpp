// unums.cpp: polynomial evaluation using unum number system
//
// Copyright (C) 2017-2023 Stillwater Supercomputing, Inc.
//
// This file is part of the MPADAO project, which is released under an MIT Open Source license.
#include <iostream>
#include <vector>
#include <cstdint>
#include <universal/number/unum/unum.hpp>

using Unum = sw::universal::unum<8, 2, uint8_t>;

Unum unumPolynomial(const std::vector<int>& coef, const Unum& x) {
	if (coef.size() < 2) {
		std::cerr << "Coefficient set is too small to represent a polynomial\n";
		return Unum(0);
	}

	// Horner's method: numerically superior and avoids pow() dependency
	size_t n = coef.size();
	Unum v = Unum(coef[n - 1]);
	for (size_t i = n - 1; i > 0; --i) {
		v = v * x + Unum(coef[i - 1]);
	}
	return v;
}
