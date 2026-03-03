/** ************************************************************************
* precision: benchmark demonstrating ULP and precision characteristics
*
* Shows the Unit in the Last Place (ULP) at various magnitudes for each
* number type, and demonstrates precision loss through iterative operations.
*
* @author:     Theodore Omtzigt
* @date:       2023-01-16
* @copyright:  Copyright (c) 2023 Stillwater Supercomputing, Inc.
* @license:    MIT Open Source license
*
* This file is part of the MPADAO project
* *************************************************************************
*/
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdint>

#include <universal/number/cfloat/cfloat.hpp>
#include <universal/number/posit/posit.hpp>

// Show ULP at various magnitudes for a Universal cfloat type
template<typename Scalar>
void UlpAnalysis(const std::string& type_name) {
	std::cout << type_name << ":\n";
	double magnitudes[] = { 0.001, 0.01, 0.1, 1.0, 10.0, 100.0, 1000.0 };
	for (double mag : magnitudes) {
		Scalar val(mag);
		Scalar val_pp = val;
		val_pp++;  // Universal cfloat ++ increments by one ULP
		double ulp_size = double(val_pp) - double(val);
		std::cout << "  at " << std::setw(8) << mag
		          << " : ULP = " << std::scientific << std::setprecision(4) << ulp_size
		          << std::fixed << '\n';
	}
	std::cout << '\n';
}

// Demonstrate precision loss through iterative multiply chain
// Start with 1.0, multiply by (1 + epsilon) N times, then divide by (1 + epsilon) N times
// Result should be 1.0 but won't be due to rounding
template<typename Scalar>
void PrecisionLossDemo(const std::string& type_name, unsigned N) {
	Scalar factor(1.0 + 1.0 / 1024.0);  // 1 + 2^-10
	Scalar val(1);

	// multiply N times
	for (unsigned i = 0; i < N; ++i) {
		val = val * factor;
	}
	// divide N times
	for (unsigned i = 0; i < N; ++i) {
		val = val / factor;
	}
	double error = std::abs(double(val) - 1.0);
	std::cout << std::setw(40) << std::left << type_name
	          << " after " << N << " mul/div roundtrips: "
	          << std::setprecision(15) << double(val)
	          << " (error = " << std::scientific << error << ")"
	          << std::fixed << '\n';
}

int main()
try {
	using namespace sw::universal;

	std::cout << "Precision Benchmark: ULP Analysis and Precision Loss\n";
	std::cout << std::string(70, '=') << "\n\n";

	using fp8  = cfloat<8, 4, std::uint8_t>;
	using fp16 = cfloat<16, 5, std::uint16_t>;
	using fp32 = cfloat<32, 8, std::uint32_t>;

	std::cout << "--- ULP at various magnitudes ---\n\n";
	UlpAnalysis<fp8>("cfloat<8,4> (fp8)");
	UlpAnalysis<fp16>("cfloat<16,5> (fp16)");
	UlpAnalysis<fp32>("cfloat<32,8> (fp32)");

	std::cout << "--- Precision loss through iterative multiply/divide chain ---\n\n";
	constexpr unsigned N = 1000;
	PrecisionLossDemo<fp16>("cfloat<16,5> (fp16)", N);
	PrecisionLossDemo<fp32>("cfloat<32,8> (fp32)", N);
	PrecisionLossDemo<posit<32, 2>>("posit<32,2>", N);
	PrecisionLossDemo<float>("native float", N);
	PrecisionLossDemo<double>("native double", N);
	std::cout << '\n';

	return EXIT_SUCCESS;
}
catch(const char* msg) {
	std::cerr << "Caught unexpected exception: " << msg << std::endl;
	return EXIT_FAILURE;
}
