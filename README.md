Implements Autograd. Core is < 500 lines of formatted c++ code. Inspired by PyTorch and Karpathy's implementation:

https://www.youtube.com/watch?v=VMj-3S1tku0&list=PLAqhIrjkxbuWI23v9cThsA9GvCAUhRvKZ

Mostly for learning.


## Build
cmake -S . -B ./build && cmake --build build

## Test
ctest --test-dir build/test

## Line count
wc -l src/scalar.hpp src/topo.hpp src/operation.hpp
