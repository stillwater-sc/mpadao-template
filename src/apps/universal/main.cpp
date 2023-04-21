/** ************************************************************************
* example: skeleton for working with Universal
*
* @author:     Theodore Omtzigt
* @date:       2023-01-04
* @copyright:  Copyright (c) 2023 Stillwater Supercomputing, Inc.
* @license:    MIT Open Source license
*
* This file is part of the Mixed Precision Iterative Refinement project
* *************************************************************************
*/
#include <iostream>
#include <iomanip>

#include <universal/number/cfloat/cfloat.hpp>

int main() 
try {
	using namespace sw::universal;

	half a, b, c;
	a = 1.5f;
	b = 0.75f;
	c = a * b;
	std::cout << a << " * " << b << " = " << c << '\n';
	std::cout << to_binary(a) << " * " << to_binary(b) << " = " << to_binary(c) << '\n';

	{
		cfloat<64, 11, std::uint32_t, false, false, false> fp64(0);
		cfloat<32, 8, std::uint32_t, false, false, false> fp32(0);
		cfloat<16, 8, std::uint32_t, false, false, false> bf16(0);
		cfloat<16, 5, std::uint32_t, false, false, false> fp16(1), fp16pp(fp16);
		cfloat<8, 5, std::uint32_t, false, false, false> fp8_5(1);
		cfloat<8, 4, std::uint32_t, false, false, false> fp8_4(1);

		std::cout << symmetry_range(fp64) << '\n';
		std::cout << symmetry_range(fp32) << '\n';
		std::cout << symmetry_range(bf16) << '\n';
		std::cout << symmetry_range(fp16) << '\n';
		std::cout << symmetry_range(fp8_5) << '\n';
		std::cout << symmetry_range(fp8_4) << '\n';

		fp16pp++;
		std::cout << to_binary(fp16) << " " << to_binary(fp16pp) << " " << to_binary(fp16pp - fp16) << '\n';
		std::cout << to_binary(ulp(fp64)) << " " << ulp(fp64) << '\n';
		std::cout << to_binary(ulp(fp32)) << " " << ulp(fp32) << '\n';
		std::cout << to_binary(ulp(bf16)) << " " << ulp(bf16) << '\n';
		std::cout << to_binary(ulp(fp16)) << " " << ulp(fp16) << '\n';
		std::cout << to_binary(ulp(fp8_5)) << " "  << ulp(fp8_5) << '\n';
		std::cout << to_binary(ulp(fp8_4)) << " " << ulp(fp8_4) << '\n';

	}
}
catch(const char* msg) {
	std::cerr << "Caught unexpected exception: " << msg << std::endl;
	return EXIT_FAILURE;
}
