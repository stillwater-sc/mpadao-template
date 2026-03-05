/** ************************************************************************
* Karatsuba multiplication demonstration
*
* Compares schoolbook O(n^2) vs Karatsuba O(n^1.585) multiplication on
* big integers of increasing size. Demonstrates the empirical crossover
* point where Karatsuba's reduced asymptotic complexity overcomes its
* higher constant factor.
*
* @author:     Theodore Omtzigt
* @date:       2025-01-15
* @copyright:  Copyright (c) 2025 Stillwater Supercomputing, Inc.
* @license:    MIT Open Source license
*
* This file is part of the MPADAO project
* *************************************************************************
*/
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <chrono>
#include <random>

#include <karatsuba.hpp>

int main()
try {
	using namespace mpadao;

	std::cout << "Karatsuba vs Schoolbook Multiplication Benchmark\n";
	std::cout << "================================================\n\n";
	std::cout << "Comparing O(n^2) schoolbook with O(n^1.585) Karatsuba multiplication\n";
	std::cout << "on random big integers of increasing size.\n\n";

	// Sizes to benchmark (number of 32-bit blocks)
	const std::vector<size_t> sizes = { 64, 128, 256, 512, 1024, 2048, 4096, 8192 };

	// Number of iterations for timing stability
	auto iterations_for_size = [](size_t n) -> int {
		if (n <= 128)  return 100;
		if (n <= 512)  return 20;
		if (n <= 2048) return 5;
		return 2;
	};

	std::mt19937 rng(42); // Fixed seed for reproducibility

	std::cout << std::setw(8) << "Blocks"
	          << std::setw(12) << "Bits"
	          << std::setw(16) << "Schoolbook(us)"
	          << std::setw(16) << "Karatsuba(us)"
	          << std::setw(12) << "Speedup"
	          << std::setw(10) << "Correct"
	          << "\n";
	std::cout << std::string(74, '-') << "\n";

	bool all_correct = true;

	for (size_t n : sizes) {
		int iters = iterations_for_size(n);

		// Generate random operands
		BigNum a = random_bignum(n, rng);
		BigNum b = random_bignum(n, rng);

		// Time schoolbook multiplication
		auto t0 = std::chrono::high_resolution_clock::now();
		BigNum result_school;
		for (int i = 0; i < iters; ++i) {
			result_school = schoolbook_multiply(a, b);
		}
		auto t1 = std::chrono::high_resolution_clock::now();
		double us_school = std::chrono::duration<double, std::micro>(t1 - t0).count() / iters;

		// Time Karatsuba multiplication
		auto t2 = std::chrono::high_resolution_clock::now();
		BigNum result_karat;
		for (int i = 0; i < iters; ++i) {
			result_karat = karatsuba_multiply(a, b);
		}
		auto t3 = std::chrono::high_resolution_clock::now();
		double us_karat = std::chrono::duration<double, std::micro>(t3 - t2).count() / iters;

		// Verify correctness
		bool correct = (result_school == result_karat);
		if (!correct) all_correct = false;

		double speedup = us_school / us_karat;

		std::cout << std::setw(8) << n
		          << std::setw(12) << (n * 32)
		          << std::setw(16) << std::fixed << std::setprecision(1) << us_school
		          << std::setw(16) << std::fixed << std::setprecision(1) << us_karat
		          << std::setw(11) << std::fixed << std::setprecision(2) << speedup << "x"
		          << std::setw(10) << (correct ? "YES" : "NO")
		          << "\n";
	}

	std::cout << std::string(74, '-') << "\n";

	if (all_correct) {
		std::cout << "\nAll results verified: Karatsuba matches schoolbook at every size.\n";
	} else {
		std::cerr << "\nERROR: Mismatch detected between schoolbook and Karatsuba results!\n";
		return EXIT_FAILURE;
	}

	std::cout << "\nNote: Karatsuba crossover threshold is 32 blocks (1024 bits).\n";
	std::cout << "Below this threshold, Karatsuba falls back to schoolbook.\n";
	std::cout << "Speedup increases with size due to O(n^1.585) vs O(n^2) scaling.\n";

	return EXIT_SUCCESS;
}
catch(const char* msg) {
	std::cerr << "Caught unexpected exception: " << msg << std::endl;
	return EXIT_FAILURE;
}
catch(const std::exception& e) {
	std::cerr << "Caught exception: " << e.what() << std::endl;
	return EXIT_FAILURE;
}
