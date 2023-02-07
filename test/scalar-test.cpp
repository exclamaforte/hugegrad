#include "scalar.hpp"
#include <gtest/gtest.h>

class ScalarTest : public ::testing::Test {
protected:
  void SetUp() override {
    x = make_scalar<int>(100);
    y = make_scalar<short>(5);
    yy = make_scalar<short>(5);
    z = make_scalar<int>(100);
    a = make_scalar<short>(100);
    b = make_scalar<int>(-100);

  }
  std::shared_ptr<ScalarValue<int>> x;
  std::shared_ptr<ScalarValue<short>> y;
  std::shared_ptr<ScalarValue<short>> yy;
  std::shared_ptr<ScalarValue<int>> z;
  std::shared_ptr<ScalarValue<short>> a;
  std::shared_ptr<ScalarValue<int>> b;

};

TEST_F(ScalarTest, equality) {
  EXPECT_EQ(x, x);
  EXPECT_EQ(x, z);
  EXPECT_EQ(z, z);
  EXPECT_EQ(x, a);
  EXPECT_NE(x, y);
}

TEST_F(ScalarTest, formatting) {
  EXPECT_EQ(fmt::format("{}", x), "Value(data=100)");
}

TEST_F(ScalarTest, addition) {
  auto result = make_scalar(100 + 100);
  auto result2 = make_scalar(5 + 5);
  EXPECT_EQ(x + z, result);
  EXPECT_EQ(y + yy, result2);
}

TEST_F(ScalarTest, multiplication) {
  auto result = make_scalar(-100 * 100);
  auto result2 = make_scalar(5 * 5);
  EXPECT_EQ(b * x, result) << fmt::format("{}, {}", b * x, result);
  EXPECT_EQ(y * yy, result2);
}

TEST_F(ScalarTest, test_children) {
  auto mul = x + z;
  EXPECT_EQ(mul->child1, mul->child2);
  auto mul2 = x * z;
  EXPECT_EQ(mul2->child1, mul2->child2);
}
