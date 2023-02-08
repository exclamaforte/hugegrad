#pragma once
#include "operation.hpp"
#include <fmt/format.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <cmath>
using namespace Operation;
namespace Scalar {

template <typename T>
struct ScalarValue {
  using WrapperType = std::shared_ptr<ScalarValue<T>>;
  WrapperType child1;
  WrapperType child2;
  OpType op;
  // aka "activation"
  T data;
  // aka "gradient"
  T grad;
  std::string label;
  // whether or not this value has been seen in the back-prop calculation before
  bool seen = false;

  void clear_back() {
    grad = 0;
    seen = false;
    if (op != OpType::None) {
      child1->clear_back();
      child2->clear_back();
    }
  }
  void step(T learning_rate)
  {
  }

  // derivative of anything with respect to itself is 1
  void compute_grad(T prev_grad = 1) {
    // TODO implement topological sort
    if (seen) {
      throw new std::runtime_error(fmt::format("node {} has been seen before, aborting", label));
    }
    seen = true;
    grad += prev_grad;
    // TODO use polymorphism and move this into Operation class
    switch (op) {
    case OpType::Add:
      child1->compute_grad(grad);
      child2->compute_grad(grad);
      break;
    case OpType::Mul:
      child1->compute_grad(child2->data * grad);
      child2->compute_grad(child1->data * grad);
      break;
    case OpType::Tanh:
      //fmt::print("arst: {}\n", 1 - my_tanh(child1->data) * my_tanh(child1->data));
      child1->compute_grad((1 - pow(my_tanh(child1->data), 2)) * grad);
      break;
    case OpType::Exp:
      // data == exp(in)
      child1->compute_grad(data * grad);
      break;
    case OpType::None:
      break;
    default:
      break;
    }
  }

  ScalarValue(T data) : data(data), op(OpType::None) {}
  ScalarValue(T data, std::string label) : data(data), label(label), op(OpType::None) {}

  ScalarValue(T data, WrapperType &child1, WrapperType &child2, OpType op)
      : data(data), op(op), child1(child1), child2(child2) {
    if (child1.get() == child2.get()) {
      throw new std::runtime_error("cannot have the same children for now");
    }
  }

  ScalarValue(T data, WrapperType &child1, WrapperType &child2, OpType op,
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
            std::shared_ptr<ScalarValue<T>> &child2, OpType op,
            std::string label) {
  return std::shared_ptr<ScalarValue<T>>(
      new ScalarValue<T>(data, child1, child2, op, label));
}
template <typename T>
std::shared_ptr<ScalarValue<T>>
make_scalar(T data, std::shared_ptr<ScalarValue<T>> &child1,
            std::shared_ptr<ScalarValue<T>> &child2, OpType op) {
  return std::shared_ptr<ScalarValue<T>>(
      new ScalarValue<T>(data, child1, child2, op));
}


template <typename T>
std::shared_ptr<ScalarValue<T>>
operator+(std::shared_ptr<ScalarValue<T>> left,
          std::shared_ptr<ScalarValue<T>> right) {
  T data = left->data + right->data;
  return make_scalar(data, left, right, OpType::Add);
}

template <typename T>
std::shared_ptr<ScalarValue<T>>
operator+(T left,
          std::shared_ptr<ScalarValue<T>> right) {
  T data = left + right->data;
  auto left_val = make_scalar(left);
  return make_scalar(data, left_val, right, OpType::Add);
}

template <typename T>
std::shared_ptr<ScalarValue<T>>
operator+(std::shared_ptr<ScalarValue<T>> left,
          T right) {
  T data = left->data + right;
  auto right_val = make_scalar(right);
  return make_scalar(data, left, right_val, OpType::Add);
}

template <typename K, typename T>
std::shared_ptr<ScalarValue<K>>
operator*(std::shared_ptr<ScalarValue<K>> left,
          std::shared_ptr<ScalarValue<T>> right) {
  K data = left->data * right->data;
  return make_scalar(data, left, right, OpType::Mul);
}

template <typename K, typename T>
std::shared_ptr<ScalarValue<K>>
operator*(K left,
          std::shared_ptr<ScalarValue<T>> right) {
  K data = left * right->data;
  auto left_val = make_scalar(left);
  return make_scalar(data, left_val, right, OpType::Mul);
}

template <typename K, typename T>
std::shared_ptr<ScalarValue<K>>
operator*(std::shared_ptr<ScalarValue<K>> left,
          T right) {
  K data = left->data * right->data;
  auto right_val = make_scalar(right);
  return make_scalar(data, left, right_val, OpType::Mul);
}

template <typename T>
pow(std::shared_ptr<T> val, T power)
{
  std::shared_ptr<ScalarValue<T>> tmp;
  return make_scalar(std::pow(val, power), val, tmp, OpType::Pow);
}
/*
template <typename T>
std::shared_ptr<ScalarValue<T>>
operator/(std::shared_ptr<ScalarValue<T>> num,
          std::shared_ptr<ScalarValue<T>> den) {
  return num * pow(den, -1);
}
*/

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

template <typename T>
std::shared_ptr<ScalarValue<T>>
tanh(std::shared_ptr<ScalarValue<T>> val) {
  std::shared_ptr<ScalarValue<T>> tmp;
  return make_scalar<T>(my_tanh(val->data), val, tmp, OpType::Tanh);
}

template <typename T>
std::shared_ptr<ScalarValue<T>>
exp(std::shared_ptr<ScalarValue<T>> val) {
  std::shared_ptr<ScalarValue<T>> tmp;
  return make_scalar<T>(my_exp(val->data), val, tmp, OpType::Exp);
}
} // namespace Scalar

template <typename T>
struct fmt::formatter<Scalar::ScalarValue<T>> : formatter<string_view> {

  template <typename FormatContext>
  auto format(const Scalar::ScalarValue<T> &sv, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    // ctx.out() is an output iterator to write to.
    return fmt::format_to(ctx.out(), "{}(data={}, grad={})", sv.label, sv.data,
                          sv.grad);
  }
};

template <typename T>
struct fmt::formatter<std::shared_ptr<Scalar::ScalarValue<T>>>
    : formatter<string_view> {

  template <typename FormatContext>
  auto format(const std::shared_ptr<Scalar::ScalarValue<T>> &sv,
              FormatContext &ctx) const -> decltype(ctx.out()) {
    // ctx.out() is an output iterator to write to.
    return fmt::format_to(ctx.out(), "{}", *sv);
  }
};
