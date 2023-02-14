#include "gen-vis.hpp"
#include "scalar.hpp"
using namespace ScalarNS;
void example1() {
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
  L->compute_grad();
  write_vis(L);
}
void example2() {
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
  write_vis(o);
}
int main() {
  example2();
}
