/** ************************************************************************
* example: skeleton for working with Universal and Abseil
*
* @author:     Theodore Omtzigt
* @date:       2023-04-11
* @copyright:  Copyright (c) 2023 Stillwater Supercomputing, Inc.
* @license:    MIT Open Source license
*
* This file is part of the Mixed Precision Iterative Refinement project
* *************************************************************************
*/
#include <iostream>
#include <iomanip>

#include <absl/log/log.h>
#include <universal/number/cfloat/cfloat.hpp>

void Kernel3() {
	sw::universal::quarter f(1.0f);
	LOG(INFO) << "Kernel3     : " << sw::universal::type_tag(f) << " : " << f;
}
void Kernel2() {
	sw::universal::half f(1.0f);
	LOG(INFO) << "Kernel2     : " << sw::universal::type_tag(f) << " : " << f;
	Kernel3();
}
void Kernel1() {
	sw::universal::single f(1.0f);
	LOG(INFO) << "Kernel1     : " << sw::universal::type_tag(f) << " : " << f;
	Kernel2();
}
void FunctionNest() {
	sw::universal::duble f(1.0f);
	LOG(INFO) << "FunctionNest: " << sw::universal::type_tag(f) << " : " << f;
	Kernel1();
}

int main() 
try {
	using namespace sw::universal;

	half a, b, c;
	a = 1.5f;
	b = 0.75f;
	c = a * b;
	std::cout << a << " * " << b << " = " << c << '\n';
	std::cout << to_binary(a) << " * " << to_binary(b) << " = " << to_binary(c) << '\n';

	LOG(INFO) << "informational message: " << c;
	LOG(WARNING) << "unexpected event";

	LOG(ERROR) << "unexpected problem, this message should provide information how to fix the problem";

	FunctionNest();

	// fatal: aborting
	LOG(FATAL) << "fatal error, terminating!";
}
catch(const char* msg) {
	std::cerr << "Caught unexpected exception: " << msg << std::endl;
	return EXIT_FAILURE;
}
catch (...) {
	std::cerr << "Caught unexpected exception: " << std::endl;
	return EXIT_FAILURE;
}
