#pragma once
// karatsuba.hpp: schoolbook and Karatsuba multiplication on big integers
//
// Implements multi-precision integer multiplication using little-endian
// base-2^32 representation. Provides both O(n^2) schoolbook and
// O(n^1.585) Karatsuba algorithms with configurable crossover threshold.
//
// Copyright (c) 2025 Stillwater Supercomputing, Inc.
// SPDX-License-Identifier: MIT
//
// This file is part of the MPADAO project

#include <vector>
#include <cstdint>
#include <algorithm>
#include <random>

namespace mpadao {

// BigNum: unsigned multi-precision integer in little-endian base-2^32
struct BigNum {
	std::vector<uint32_t> blocks; // blocks[0] = least significant

	BigNum() = default;
	explicit BigNum(size_t n, uint32_t val = 0) : blocks(n, val) {}
	explicit BigNum(std::vector<uint32_t> b) : blocks(std::move(b)) {}

	size_t size() const { return blocks.size(); }

	bool operator==(const BigNum& rhs) const {
		return blocks == rhs.blocks;
	}
	bool operator!=(const BigNum& rhs) const {
		return blocks != rhs.blocks;
	}
};

// Remove leading zero blocks (preserving at least one block)
inline void trim_leading_zeros(BigNum& x) {
	while (x.blocks.size() > 1 && x.blocks.back() == 0) {
		x.blocks.pop_back();
	}
}

// Add two BigNums, returning result
inline BigNum add(const BigNum& a, const BigNum& b) {
	size_t n = std::max(a.size(), b.size());
	BigNum result(n + 1);
	uint64_t carry = 0;
	for (size_t i = 0; i < n; ++i) {
		uint64_t sum = carry;
		if (i < a.size()) sum += a.blocks[i];
		if (i < b.size()) sum += b.blocks[i];
		result.blocks[i] = static_cast<uint32_t>(sum);
		carry = sum >> 32;
	}
	result.blocks[n] = static_cast<uint32_t>(carry);
	trim_leading_zeros(result);
	return result;
}

// Subtract b from a (assumes a >= b)
inline BigNum subtract(const BigNum& a, const BigNum& b) {
	size_t n = a.size();
	BigNum result(n);
	int64_t borrow = 0;
	for (size_t i = 0; i < n; ++i) {
		int64_t diff = static_cast<int64_t>(a.blocks[i]) - borrow;
		if (i < b.size()) diff -= b.blocks[i];
		if (diff < 0) {
			diff += (1LL << 32);
			borrow = 1;
		} else {
			borrow = 0;
		}
		result.blocks[i] = static_cast<uint32_t>(diff);
	}
	trim_leading_zeros(result);
	return result;
}

// Schoolbook multiplication: O(n^2)
inline BigNum schoolbook_multiply(const BigNum& a, const BigNum& b) {
	size_t na = a.size(), nb = b.size();
	BigNum result(na + nb, 0);
	for (size_t i = 0; i < na; ++i) {
		uint64_t carry = 0;
		for (size_t j = 0; j < nb; ++j) {
			uint64_t prod = static_cast<uint64_t>(a.blocks[i])
			              * static_cast<uint64_t>(b.blocks[j])
			              + result.blocks[i + j] + carry;
			result.blocks[i + j] = static_cast<uint32_t>(prod);
			carry = prod >> 32;
		}
		result.blocks[i + nb] += static_cast<uint32_t>(carry);
	}
	trim_leading_zeros(result);
	return result;
}

// Extract a sub-range of blocks [lo, hi)
inline BigNum slice(const BigNum& x, size_t lo, size_t hi) {
	if (lo >= x.size()) return BigNum(1, 0);
	hi = std::min(hi, x.size());
	BigNum result(hi - lo);
	for (size_t i = lo; i < hi; ++i) {
		result.blocks[i - lo] = x.blocks[i];
	}
	trim_leading_zeros(result);
	return result;
}

// Shift left by n blocks (multiply by base^n)
inline BigNum shift_blocks(const BigNum& x, size_t n) {
	if (x.size() == 1 && x.blocks[0] == 0) return x;
	BigNum result(x.size() + n, 0);
	for (size_t i = 0; i < x.size(); ++i) {
		result.blocks[i + n] = x.blocks[i];
	}
	return result;
}

// Compare: returns -1, 0, or 1
inline int compare(const BigNum& a, const BigNum& b) {
	if (a.size() != b.size()) return (a.size() < b.size()) ? -1 : 1;
	for (size_t i = a.size(); i-- > 0;) {
		if (a.blocks[i] < b.blocks[i]) return -1;
		if (a.blocks[i] > b.blocks[i]) return 1;
	}
	return 0;
}

// Signed subtract: returns |a-b| and sets sign
inline BigNum signed_subtract(const BigNum& a, const BigNum& b, bool& negative) {
	if (compare(a, b) >= 0) {
		negative = false;
		return subtract(a, b);
	}
	negative = true;
	return subtract(b, a);
}

// Karatsuba multiplication: O(n^1.585)
// Falls back to schoolbook when size <= crossover
inline BigNum karatsuba_multiply(const BigNum& a, const BigNum& b, size_t crossover = 32) {
	size_t n = std::max(a.size(), b.size());

	// Fall back to schoolbook for small inputs
	if (n <= crossover) {
		return schoolbook_multiply(a, b);
	}

	size_t half = n / 2;

	// Split: a = a1 * B^half + a0, b = b1 * B^half + b0
	BigNum a0 = slice(a, 0, half);
	BigNum a1 = slice(a, half, a.size());
	BigNum b0 = slice(b, 0, half);
	BigNum b1 = slice(b, half, b.size());

	// Three recursive multiplications (instead of four)
	BigNum z0 = karatsuba_multiply(a0, b0, crossover);       // a0 * b0
	BigNum z2 = karatsuba_multiply(a1, b1, crossover);       // a1 * b1

	// z1 = (a0+a1)(b0+b1) - z0 - z2 = a0*b1 + a1*b0
	// Handle possible sign issues with Karatsuba variant
	BigNum a_sum = add(a0, a1);
	BigNum b_sum = add(b0, b1);
	BigNum z1_full = karatsuba_multiply(a_sum, b_sum, crossover);
	BigNum z1 = subtract(subtract(z1_full, z0), z2);

	// result = z2 * B^(2*half) + z1 * B^half + z0
	BigNum result = add(add(shift_blocks(z2, 2 * half), shift_blocks(z1, half)), z0);
	trim_leading_zeros(result);
	return result;
}

// Generate a random BigNum with the given number of blocks
inline BigNum random_bignum(size_t num_blocks, std::mt19937& rng) {
	BigNum result(num_blocks);
	std::uniform_int_distribution<uint32_t> dist;
	for (size_t i = 0; i < num_blocks; ++i) {
		result.blocks[i] = dist(rng);
	}
	// Ensure MSB is non-zero
	if (num_blocks > 0 && result.blocks[num_blocks - 1] == 0) {
		result.blocks[num_blocks - 1] = 1;
	}
	return result;
}

} // namespace mpadao
