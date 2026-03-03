#include <version.hpp>
#include <gtest/gtest.h>
#include <string>
#include <regex>
#include <cstdint>

#include <universal/number/cfloat/cfloat.hpp>

// Test that the version library returns a valid semver string
TEST(VersionTest, ReturnsValidSemver) {
	std::string ver = mpadao::get_semver();
	EXPECT_FALSE(ver.empty());
	// Verify X.Y.Z format
	std::regex semver_re(R"(\d+\.\d+\.\d+)");
	EXPECT_TRUE(std::regex_match(ver, semver_re)) << "get_semver() returned: " << ver;
}

// Test basic cfloat arithmetic
TEST(CfloatTest, BasicArithmetic) {
	using namespace sw::universal;
	cfloat<32, 8, uint32_t> a(1.5), b(0.75);
	cfloat<32, 8, uint32_t> c = a * b;
	EXPECT_DOUBLE_EQ(double(c), 1.125);
}

// Test quadratic formula: x^2 - 5x + 6 = 0 has roots 2 and 3
TEST(QuadraticTest, SolveSimpleQuadratic) {
	using namespace sw::universal;
	using Scalar = cfloat<32, 8, uint32_t>;

	Scalar a(1), b(-5), c(6);
	Scalar discriminant = b * b - Scalar(4) * a * c;
	Scalar sqrt_disc = sqrt(discriminant);
	Scalar root1 = (-b - sqrt_disc) / (Scalar(2) * a);
	Scalar root2 = (-b + sqrt_disc) / (Scalar(2) * a);

	// Roots should be 2 and 3 (root1 <= root2)
	EXPECT_NEAR(double(root1), 2.0, 1e-6);
	EXPECT_NEAR(double(root2), 3.0, 1e-6);
}
