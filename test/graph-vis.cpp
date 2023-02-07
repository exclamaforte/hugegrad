#include "gen-vis.hpp"
#include "scalar.hpp"
int main() {
  auto x = make_scalar<int>(10);
  auto y = make_scalar<int>(-20);
  auto xx = make_scalar<int>(-30);
  auto z = x * y + xx;
  z->backprop(1);
  write_vis(z);
}
