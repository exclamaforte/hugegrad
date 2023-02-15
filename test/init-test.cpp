#include "initialization.hpp"
#include <gtest/gtest.h>

class InitTest : public ::testing::Test {
protected:
  void SetUp() override {
  }
};

TEST_F(InitTest, zero) {
  EXPECT_EQ(zero_singleton<int>(), 0);
  EXPECT_EQ(zero_singleton<float>(), 0);
  EXPECT_EQ(zero_singleton<double>(), 0);
  EXPECT_EQ(zero_singleton<short>(), 0);
}

// TODO duplicate for different data types
TEST_F(InitTest, uniform_float) {
  constexpr std::size_t size = 1000;
  std::array<float, size> test_vector;
  UniformFloatInit<float> myinit(1.0, 6.0);
  for (int i = 0; i < test_vector.size(); ++i) {
    test_vector[i] = myinit();
    EXPECT_GE(test_vector[i], 0.99);
    EXPECT_LE(test_vector[i], 6.01);
  }
}

TEST_F(InitTest, uniform_int) {
  constexpr std::size_t size = 1000;
  std::array<std::int16_t, size> test_vector;
  UniformIntInit<std::int16_t> myinit(-6, -1);
  for (int i = 0; i < test_vector.size(); ++i) {
    test_vector[i] = myinit();
    EXPECT_GE(test_vector[i], -7);
    EXPECT_LE(test_vector[i], 0);
  }
}
