/** ************************************************************************
* performance: skeleton to develop algorithms with high performance
*
* @author:     Theodore Omtzigt
* @date:       2023-01-16
* @copyright:  Copyright (c) 2023 Stillwater Supercomputing, Inc.
* @license:    MIT Open Source license
*
* This file is part of the Mixed Precision Iterative Refinement project
* *************************************************************************
*/
#include <iostream>
#include <iomanip>
#include <universal/native/ieee754.hpp>
// #include <universal/benchmark/performance_runner.hpp>  future location of the performance running after v3.67
#include <universal/verification/performance_runner.hpp>
#include <universal/verification/test_case.hpp>
#include <universal/number/cfloat/cfloat.hpp>
#include <universal/blas/blas.hpp>
#include <boost/numeric/mtl/mtl.hpp>

namespace sw::universal {
	using xtndd = sw::universal::cfloat<80, 11, std::uint32_t, true, false, false>;
	using fp80 = xtndd;
}

template<typename Real>
void ReportMul(float fa = 1.5f, float fb = 0.75f) {
	Real a, b, c;
	a = fa;
	b = fb;
	c = a * b;
	sw::universal::ReportBinaryOperation(a, " * ", b, c);
}

void Compare() {
	using namespace sw::universal;

	float fa = 1.5f;
	float fb = 0.75f;

	TestCase<duble, float>(TestCaseOperator::MUL, fa, fb);
	TestCase<xtndd, float>(TestCaseOperator::MUL, fa, fb);
	TestCase<quad, float>(TestCaseOperator::MUL, fa, fb);

	ReportMul<duble>(fa, fb);
	ReportMul<xtndd>(fa, fb);
	ReportMul<quad>(fa, fb);
}

constexpr unsigned nrElements = 1024;
static std::vector<float> fva(nrElements), fvb(nrElements);
static std::vector<sw::universal::fp8> qrpva(nrElements), qrpvb(nrElements);
static std::vector<sw::universal::fp16> hpva(nrElements), hpvb(nrElements);
static std::vector<sw::universal::fp32> spva(nrElements), spvb(nrElements);
static std::vector<sw::universal::fp64> dpva(nrElements), dpvb(nrElements);
static std::vector<sw::universal::fp80> xpva(nrElements), xpvb(nrElements);
static std::vector<sw::universal::fp128> qpva(nrElements), qpvb(nrElements);


// Generic workload for testing dot products
template<typename Scalar>
void FloatDotPerformanceWorkload(size_t NR_OPS) {
	Scalar d{};
	for (size_t i = 0; i < NR_OPS; ++i) {
		d += sw::universal::blas::dot(fva, fvb);
	}
	if (d == 0.0f) std::cout << "vectors are perpendicular" << '\n';
}

template<typename Scalar>
void FP8DotPerformanceWorkload(size_t NR_OPS) {
	Scalar d{};
	for (size_t i = 0; i < NR_OPS; ++i) {
		d += sw::universal::blas::dot(qrpva, qrpvb);
	}
	if (d == 0.0f) std::cout << "vectors are perpendicular" << '\n';
}

template<typename Scalar>
void FP16DotPerformanceWorkload(size_t NR_OPS) {
	Scalar d{};
	for (size_t i = 0; i < NR_OPS; ++i) {
		d += sw::universal::blas::dot(hpva, hpvb);
	}
	if (d == 0.0f) std::cout << "vectors are perpendicular" << '\n';
}

template<typename Scalar>
void FP32DotPerformanceWorkload(size_t NR_OPS) {
	Scalar d{};
	for (size_t i = 0; i < NR_OPS; ++i) {
		d += sw::universal::blas::dot(spva, spvb);
	}
	if (d == 0.0f) std::cout << "vectors are perpendicular" << '\n';
}

int main() 
try {
	using namespace sw::universal;

	size_t NR_OPS = 1000;

	PerformanceRunner("float dot product", FloatDotPerformanceWorkload<float>, NR_OPS);
	PerformanceRunner("fp8   dot product", FP8DotPerformanceWorkload<fp8>, NR_OPS);
	PerformanceRunner("fp16  dot product", FP16DotPerformanceWorkload<fp16>, NR_OPS);
	PerformanceRunner("fp32  dot product", FP32DotPerformanceWorkload<fp32>, NR_OPS);
}
catch(const char* msg) {
	std::cerr << "Caught unexcpected exception: " << msg << std::endl;
	return EXIT_FAILURE;
}
