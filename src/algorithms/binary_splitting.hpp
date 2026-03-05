#pragma once
// binary_splitting.hpp: generic binary splitting for hypergeometric series
//
// Binary splitting computes sums of the form:
//   S = sum_{k=a}^{b-1} a(k) * p(a..k) / q(a..k)
//
// by recursively splitting the range [a,b) at the midpoint and combining
// partial results using only integer-like multiplications (no divisions
// until the final step). This achieves O(n log^2 n) complexity for n terms
// when paired with sub-quadratic multiplication.
//
// The algorithm maintains three quantities P, Q, T at each node:
//   P(a,b) = product of p(k) for k in [a,b)
//   Q(a,b) = product of q(k) for k in [a,b)
//   T(a,b) = Q(a,b) * S(a,b) * P_prefactor  (the "numerator sum")
//
// Combination rule at split point m:
//   P(a,b) = P(a,m) * P(m,b)
//   Q(a,b) = Q(a,m) * Q(m,b)
//   T(a,b) = Q(m,b) * T(a,m) + P(a,m) * T(m,b)
//
// Copyright (c) 2025 Stillwater Supercomputing, Inc.
// SPDX-License-Identifier: MIT
//
// This file is part of the MPADAO project

#include <functional>
#include <tuple>

namespace mpadao {

// PQT triple returned by binary_split
template<typename Real>
struct PQT {
	Real P, Q, T;
};

// binary_split: recursively split [a, b) and combine PQT triples
//
// p_func(k) returns P contribution at term k
// q_func(k) returns Q contribution at term k
// a_func(k) returns the series coefficient a(k) (sign included)
//
// Base case (single term k=a):
//   P = p_func(a), Q = q_func(a), T = a_func(a) * P
//
// The final sum is T(0,n) / (Q(0,n) * additional_factor)
// where additional_factor depends on the specific series.
template<typename Real, typename PFunc, typename QFunc, typename AFunc>
PQT<Real> binary_split(int a, int b, PFunc p_func, QFunc q_func, AFunc a_func) {
	if (b - a == 1) {
		Real P = p_func(a);
		Real Q = q_func(a);
		Real T = a_func(a) * P;
		return { P, Q, T };
	}

	int m = (a + b) / 2;
	auto left  = binary_split<Real>(a, m, p_func, q_func, a_func);
	auto right = binary_split<Real>(m, b, p_func, q_func, a_func);

	Real P = left.P * right.P;
	Real Q = left.Q * right.Q;
	Real T = right.Q * left.T + left.P * right.T;

	return { P, Q, T };
}

} // namespace mpadao
