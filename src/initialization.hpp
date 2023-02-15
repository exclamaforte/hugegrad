#pragma once
#include <random>
#include <functional>
#include <cassert>
#include <span>

template <typename T>
struct ZeroInit {
  T init() { return 0; }
  T operator()() { return init(); }
  void init_range(T *to_init, std::size_t size) {
    for (std::size_t i = 0; i < size; ++i) {
      *to_init = init();
      ++to_init;
    }
  }
};

template <typename T>
static ZeroInit<T> zero_singleton = ZeroInit<T>();

template <std::floating_point T>
struct UniformFloatInit {
  std::default_random_engine generator;
  std::uniform_real_distribution<T> distribution;
  UniformFloatInit(T low, T high) : distribution(low, high) {
    assert(low <= high);
  }
  T init() { return distribution(generator); }
  T operator()() { return init(); }
  void init_range(T *to_init, std::size_t size) {
    for (std::size_t i = 0; i < size; ++i) {
      *to_init = init();
      ++to_init;
    }
  }
};

template <std::integral T>
struct UniformIntInit {
  std::default_random_engine generator;
  std::uniform_int_distribution<T> distribution;
  UniformIntInit(T low, T high) : distribution(low, high) {
    assert(low <= high);
  }
  T init() { return distribution(generator); }
  T operator()() { return init(); }
  void init_range(T *to_init, std::size_t size) {
    for (std::size_t i = 0; i < size; ++i) {
      *to_init = init();
      ++to_init;
    }
  }
};
