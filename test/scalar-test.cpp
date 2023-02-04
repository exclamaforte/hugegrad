#include "scalar.hpp"
#include <gtest/gtest.h>

class ScalarTest : public ::testing::Test {
protected:
  void SetUp() override {
    x = ScalarValue<int>(100);
    y = ScalarValue<short>(5);
    z = ScalarValue<int>(100);
    a = ScalarValue<short>(100);
    b = ScalarValue<int>(-100);
  }
  ScalarValue<int> x;
  ScalarValue<short> y;
  ScalarValue<int> z;
  ScalarValue<short> a;
  ScalarValue<int> b;
};


TEST_F(ScalarTest, formatting) {
  EXPECT_EQ(fmt::format("{}", x), "Value(data=100)");
}

TEST_F(ScalarTest, equality) {
  EXPECT_EQ(x, x);
  EXPECT_EQ(x, z);
  EXPECT_EQ(z, z);
  EXPECT_EQ(x, a);
  EXPECT_NE(x, y);
}

TEST_F(ScalarTest, addition) {
  ScalarValue<long> result(100 + 5);
  ScalarValue<short> result2(5 + 5);
  EXPECT_EQ(x + y, result);
  EXPECT_EQ(y + y, result2);
}

TEST_F(ScalarTest, multiplication) {
  ScalarValue<int> result(-100 * 5);
  ScalarValue<short> result2(5 * 5);
  EXPECT_EQ(b * y, result) << fmt::format("{}, {}", x * y, result);
  EXPECT_EQ(y * y, result2);
}
