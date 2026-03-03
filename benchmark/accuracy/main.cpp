/** ************************************************************************
* accuracy: benchmark measuring numerical accuracy across number types
*
* Computes the Basel series sum(1/k^2, k=1..N) which converges to pi^2/6.
* Measures absolute and relative error vs a high-precision reference value.
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

// Reference value: pi^2/6 to high precision
static const double BASEL_REFERENCE = M_PI * M_PI / 6.0;

template<typename Scalar>
void BaselAccuracyBenchmark(const std::string& type_name, unsigned N) {
	Scalar sum(0);
	for (unsigned k = 1; k <= N; ++k) {
		Scalar k_val(k);
		sum += Scalar(1) / (k_val * k_val);
	}
	double result = double(sum);
	double abs_error = std::abs(result - BASEL_REFERENCE);
	double rel_error = abs_error / BASEL_REFERENCE;

	std::cout << std::setw(40) << std::left << type_name
	          << " sum = " << std::setw(18) << std::setprecision(12) << result
	          << " abs_err = " << std::setw(14) << std::scientific << abs_error
	          << " rel_err = " << std::setw(14) << rel_error
	          << std::fixed << '\n';
}

int main()
try {
	using namespace sw::universal;

	std::cout << "Basel Series Accuracy Benchmark: sum(1/k^2, k=1..N) -> pi^2/6\n";
	std::cout << "Reference value: " << std::setprecision(15) << BASEL_REFERENCE << "\n\n";

	constexpr unsigned N = 10000;
	std::cout << "N = " << N << " terms\n";
	std::cout << std::string(110, '-') << '\n';

	using fp8  = cfloat<8, 4, std::uint8_t>;
	using fp16 = cfloat<16, 5, std::uint16_t>;
	using fp32 = cfloat<32, 8, std::uint32_t>;
	using fp64 = cfloat<64, 11, std::uint32_t>;

	BaselAccuracyBenchmark<fp8>("cfloat<8,4> (fp8)", N);
	BaselAccuracyBenchmark<fp16>("cfloat<16,5> (fp16)", N);
	BaselAccuracyBenchmark<fp32>("cfloat<32,8> (fp32)", N);
	BaselAccuracyBenchmark<fp64>("cfloat<64,11> (fp64)", N);
	BaselAccuracyBenchmark<posit<32, 2>>("posit<32,2>", N);
	BaselAccuracyBenchmark<float>("native float", N);
	BaselAccuracyBenchmark<double>("native double", N);

	std::cout << std::string(110, '-') << '\n';

	return EXIT_SUCCESS;
}
catch(const char* msg) {
	std::cerr << "Caught unexpected exception: " << msg << std::endl;
	return EXIT_FAILURE;
}
