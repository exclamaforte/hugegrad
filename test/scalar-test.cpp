#include "scalar.hpp"
#include <gtest/gtest.h>
using namespace Scalar;

class ScalarTest : public ::testing::Test {
protected:
  void SetUp() override {
    x = make_scalar<int>(100, "x");
    y = make_scalar<short>(5, "y");
    yy = make_scalar<short>(5, "yy");
    z = make_scalar<int>(100, "z");
    a = make_scalar<short>(100, "a");
    b = make_scalar<int>(-100, "b");

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
  EXPECT_EQ(fmt::format("{}", x), "x(data=100, grad=0)");
}

TEST_F(ScalarTest, addition) {
  auto result = make_scalar(100 + 100);
  auto result2 = make_scalar(5 + 5);
  EXPECT_EQ(x + z, result);
  EXPECT_EQ(y + yy, result2);
}

TEST_F(ScalarTest, constant_addition) {
  auto result = make_scalar(100 + 1);
  auto result2 = make_scalar(5 + 2);
  EXPECT_EQ(x + 1, result);
  EXPECT_EQ(static_cast<short>(2) + y, result2);
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
TEST_F(ScalarTest, exp) {
  auto x = make_scalar<double>(1.0);
  auto e = exp(x);
  auto ee = make_scalar<double>(std::exp(1.0));
  EXPECT_EQ(e, ee);
}
TEST_F(ScalarTest, pow) {
  auto v = pow(x, 2);
  int result = 10000;
  EXPECT_EQ(v, result);
}

TEST_F(ScalarTest, div) {
}

TEST_F(ScalarTest, backprop1) {
  auto a = make_scalar<float>(2.0, "a");
  auto b = make_scalar<float>(-3.0, "b");
  auto c = make_scalar<float>(10.0, "c");
  auto e = a * b;
  e->label = "e";
  auto d = e + c;
  d->label = "d";
  auto f = make_scalar<float>(-2.0, "f");
  auto L = d * f;
  L->label = "L";
  L->compute_grad(1.0);
  EXPECT_FLOAT_EQ(L->grad, 1.0);
  EXPECT_FLOAT_EQ(d->grad, -2.0);
  EXPECT_FLOAT_EQ(f->grad, 4.0);
  EXPECT_FLOAT_EQ(c->grad, -2.0);
  EXPECT_FLOAT_EQ(e->grad, -2.0);
  EXPECT_FLOAT_EQ(b->grad, -4.0);
  EXPECT_FLOAT_EQ(a->grad, 6.0);
}

TEST_F(ScalarTest, backprop2) {
  auto x1 = make_scalar<float>(2.0, "x1");
  auto x2 = make_scalar<float>(0.0, "x2");
  auto w1 = make_scalar<float>(-3.0, "w1");
  auto w2 = make_scalar<float>(1.0, "w2");
  auto b = make_scalar<float>(6.8813735870195432, "b");
  auto x1w1 = x1 * w1;
  x1w1->label = "x1w1";
  auto x2w2 = x2 * w2;
  x2w2->label = "x2w2";
  auto sum = x1w1 + x2w2;
  sum->label = "x1w1 + x2w2";
  auto n = sum + b;
  n->label = "x1w1 + x2w2 + b";
  auto o = tanh(n);
  o->label = "output";
  o->compute_grad();
  EXPECT_FLOAT_EQ(o->data, 0.70710671);
  EXPECT_FLOAT_EQ(o->grad, 1.0);
  EXPECT_FLOAT_EQ(n->grad, 0.5);
  EXPECT_FLOAT_EQ(b->grad, 0.5);
  EXPECT_FLOAT_EQ(sum->grad, 0.5);
  EXPECT_FLOAT_EQ(x1w1->grad, 0.5);
  EXPECT_FLOAT_EQ(x2w2->grad, 0.5);
  EXPECT_FLOAT_EQ(x2->grad, 0.5);
  EXPECT_FLOAT_EQ(w2->grad, 0.0);
  EXPECT_FLOAT_EQ(x1->grad, -1.5);
  EXPECT_FLOAT_EQ(w1->grad, 1.0);
}
