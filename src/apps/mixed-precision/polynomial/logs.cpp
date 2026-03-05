// logs.cpp: polynomial evaluation using logarithmic number system
//
// Copyright (C) 2017-2023 Stillwater Supercomputing, Inc.
//
// This file is part of the MPADAO project, which is released under an MIT Open Source license.
#include <iostream>
#include <vector>
#include <cstdint>
#include <universal/number/lns/lns.hpp>

using LNS8 = sw::universal::lns<8, 2, std::uint8_t>;

LNS8 lnsPolynomial(const std::vector<int>& coef, const LNS8& x) {
	if (coef.size() < 2) {
		std::cerr << "Coefficient set is too small to represent a polynomial\n";
		return LNS8(0);
	}

	// Horner's method: numerically superior and avoids pow() dependency
	size_t n = coef.size();
	LNS8 v = LNS8(coef[n - 1]);
	for (size_t i = n - 1; i > 0; --i) {
		v = v * x + LNS8(coef[i - 1]);
	}
	return v;
}
