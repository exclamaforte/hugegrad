#pragma once
#include <assert.h>
#include "operation.hpp"
#include <fmt/format.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>

template <typename T> struct ScalarValue {
  using WrapperType = std::shared_ptr<ScalarValue<T>>;
  WrapperType child1;
  WrapperType child2;
  Operation op;
  // aka "activation"
  T data;
  // aka "gradient"
  T grad;
  std::string label;
  // whether or not this value has been seen in the back-prop calculation before
  bool seen;

  void clear_back() {
    grad = 0;
    seen = false;
    if (op != Operation::None) {
      child1->clear_back();
      child2->clear_back();
    }
  }
  void step(T learning_rate) {
  }

  // call on the final node in the computation
  // TODO what happens when a variable is used in multiple "paths" of
  // computation
  // derivative of anything with respect to itself is 1
  void compute_grad(T prev_grad = 1) {
    if (seen) {
      throw new std::runtime_error(fmt::format("node {} has been seen before, aborting", label));
    }
    seen = true;
    grad += prev_grad;
    switch (op) {
    case Operation::Add:
      child1->backprop(grad);
      child2->backprop(grad);
      break;
    case Operation::Mul:
      child1->backprop(child2->data * grad);
      child2->backprop(child1->data * grad);
      break;
    case Operation::None:
      break;
    default:
      grad = 0;
      break;
    }
  }

  ScalarValue(T data) : data(data), op(Operation::None) {}
  ScalarValue(T data, std::string label) : data(data), label(label), op(Operation::None) {}

  ScalarValue(T data, WrapperType &child1, WrapperType &child2, Operation op)
      : data(data), op(op), child1(child1), child2(child2) {
    if (child1.get() == child2.get()) {
      throw new std::runtime_error("cannot have the same children for now");
    }
  }

  ScalarValue(T data, WrapperType &child1, WrapperType &child2, Operation op,
              std::string &label)
      : data(data), op(op), child1(child1), child2(child2), label(label) {
    if (child1.get() == child2.get()) {
      throw new std::runtime_error("cannot have the same children for now");
    }
  }
  ScalarValue() = default;
};

template <typename T> std::shared_ptr<ScalarValue<T>> make_scalar(T data) {
  return std::shared_ptr<ScalarValue<T>>(new ScalarValue<T>(data));
}

template <typename T> std::shared_ptr<ScalarValue<T>> make_scalar(T data, std::string label) {
  return std::shared_ptr<ScalarValue<T>>(new ScalarValue<T>(data, label));
}

template <typename T>
std::shared_ptr<ScalarValue<T>>
make_scalar(T data, std::shared_ptr<ScalarValue<T>> &child1,
            std::shared_ptr<ScalarValue<T>> &child2, Operation op,
            std::string label) {
  return std::shared_ptr<ScalarValue<T>>(
      new ScalarValue<T>(data, child1, child2, op, label));
}
template <typename T>
std::shared_ptr<ScalarValue<T>>
make_scalar(T data, std::shared_ptr<ScalarValue<T>> &child1,
            std::shared_ptr<ScalarValue<T>> &child2, Operation op) {
  return std::shared_ptr<ScalarValue<T>>(
      new ScalarValue<T>(data, child1, child2, op));
}

template <typename T>
struct fmt::formatter<ScalarValue<T>> : formatter<string_view> {

  template <typename FormatContext>
  auto format(const ScalarValue<T> &sv, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    // ctx.out() is an output iterator to write to.
    return fmt::format_to(ctx.out(), "{}(data={}, grad={})", sv.label, sv.data, sv.grad);
  }
};

template <typename T>
struct fmt::formatter<std::shared_ptr<ScalarValue<T>>>
    : formatter<string_view> {

  template <typename FormatContext>
  auto format(const std::shared_ptr<ScalarValue<T>> &sv,
              FormatContext &ctx) const -> decltype(ctx.out()) {
    // ctx.out() is an output iterator to write to.
    return fmt::format_to(ctx.out(), "{}", *sv);
  }
};

template <typename T>
std::shared_ptr<ScalarValue<T>>
operator+(std::shared_ptr<ScalarValue<T>> left,
          std::shared_ptr<ScalarValue<T>> right) {
  T data = left->data + right->data;
  return make_scalar(data, left, right, Operation::Add);
}

template <typename K, typename T>
std::shared_ptr<ScalarValue<K>>
operator*(std::shared_ptr<ScalarValue<K>> left,
          std::shared_ptr<ScalarValue<T>> right) {
  K data = left->data * right->data;
  return make_scalar(data, left, right, Operation::Mul);
}

template <typename K, typename T>
bool operator==(const std::shared_ptr<ScalarValue<K>> &left,
                const std::shared_ptr<ScalarValue<T>> &right) {
  return left->data == right->data;
}

template <typename K, typename T>
bool operator!=(const std::shared_ptr<ScalarValue<K>> &left,
                const std::shared_ptr<ScalarValue<T>> &right) {
  return left->data != right->data;
}
