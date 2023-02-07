#include "gen-vis.hpp"
#include "scalar.hpp"
int main() {
  auto a = make_scalar<float>(2.0, "a");
  auto b = make_scalar<float>(-3.0, "b");
  auto c = make_scalar<float>(10.0, "c");
  auto e = a * b; e->label = "e";
  auto d = e + c; d->label = "d";
  auto f = make_scalar<float>(-2.0, "f");
  auto L = d * f; L->label = "L";
  L->backprop(1);
  write_vis(L);
}
