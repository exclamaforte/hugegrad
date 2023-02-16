Implements Autograd. Core is < 500 lines of formatted c++ code. Inspired by PyTorch and Karpathy's implementation:

https://www.youtube.com/watch?v=VMj-3S1tku0&list=PLAqhIrjkxbuWI23v9cThsA9GvCAUhRvKZ

Mostly for learning.


## Build
cmake -S . -B ./build && cmake --build build

## Test
ctest --test-dir build/test

## Usage
Build the computation:
```cpp
  auto x1 = make_scalar<float>(2.0, "x1");
  auto x2 = make_scalar<float>(0.0, "x2");
  auto w1 = make_scalar<float>(-3.0, "w1");
  auto w2 = make_scalar<float>(1.0, "w2");
  auto b = make_scalar<float>(6.8813735870195432, "b");
  auto x1w1 = x1 * w1;
  auto x2w2 = x2 * w2;
  auto sum = x1w1 + x2w2;
  auto n = sum + b;
  auto o = tanh(n);
  o->label = "output";
```

Back-prop the gradients:
```cpp
backpropagate({o});
```

Visualize the computation via graph-vis:
```cpp
write_vis(o);
```
## Line count
wc -l src/scalar.hpp src/topo.hpp src/operation.hpp
