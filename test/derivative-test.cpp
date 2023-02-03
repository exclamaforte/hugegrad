#include "derivative.hpp"
#include <gtest/gtest.h>
#include <functional>

class DerivativeTest : public ::testing::Test
{
protected:
  void SetUp() override {
    square = [](float x) { return x * x; };
    square_p = derivative<float>(square);
    cube = [](float x) { return x * x * x; };
    cube_p = derivative<float>(cube);
  }
  std::function<float(float)> square;
  std::function<float(float)> square_p;
  std::function<float(float)> cube;
  std::function<float(float)> cube_p;
};

TEST_F(DerivativeTest, XSquaredFloat0) { EXPECT_NEAR(square_p(0.0), 0.0, 0.0001); }
TEST_F(DerivativeTest, XSquaredFloat1) {EXPECT_NEAR(square_p(1.0), 2.0, 0.0001);}
TEST_F(DerivativeTest, XSquaredFloat2) { EXPECT_NEAR(square_p(2.0), 4.0, 0.0001); }
TEST_F(DerivativeTest, XSquaredFloatn1) { EXPECT_NEAR(square_p(-1.0), -2.0, 0.0001); }
TEST_F(DerivativeTest, XSquaredFloatn2) { EXPECT_NEAR(square_p(-2.0), -4.0, 0.0001); }

TEST_F(DerivativeTest, XCubeFloat0) {
  EXPECT_NEAR(cube_p(0.0), 0.0, 0.0001);
}
TEST_F(DerivativeTest, XCubeFloat1) {
  EXPECT_NEAR(cube_p(1.0), 3.0, 0.0001);
}
TEST_F(DerivativeTest, XCubeFloat2) {
  EXPECT_NEAR(cube_p(2.0), 12.0, 0.001);
}
TEST_F(DerivativeTest, XCubeFloatn1) {
  EXPECT_NEAR(cube_p(-1.0), 3.0, 0.0001);
}
TEST_F(DerivativeTest, XCubeFloatn2) {
  EXPECT_NEAR(cube_p(-2.0), 12.0, 0.001);
}
