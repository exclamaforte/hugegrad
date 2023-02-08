#pragma once
#include "operation.hpp"
#include <fmt/format.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <cmath>
namespace Scalar {

template <typename T>
struct ScalarValue {
  using WrapperType = std::shared_ptr<ScalarValue<T>>;
  WrapperType child1;
  WrapperType child2;
  // TODO make this a pointer
  Operation::Operation<T>* op;
  // aka "activation"
  T data = 0;
  // aka "gradient"
  T grad = 0;
  std::string label;
  // whether or not this value has been seen in the back-prop calculation before
  bool seen = false;

  void clear_back() {
    grad = 0;
    seen = false;
    if (op->type != Operation::OpType::NONE) {
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
    switch (op->type) {
      // binary
    case Operation::OpType::BINARY:
      child1->compute_grad(op->backward(grad, child1->data, child2->data));
      child2->compute_grad(op->backward(grad, child2->data, child1->data));
      break;
    case Operation::OpType::UNARY:
      child1->compute_grad(op->backward(grad, child1->data, child2->data));
      break;
    case Operation::OpType::NONE:
      break;
    default:
      break;
    }
  }

  ScalarValue(T data) : data(data), op(Operation::none_ptr<T>) {}
  ScalarValue(T data, std::string label) : data(data), label(label), op(Operation::none_ptr<T>) {}

  ScalarValue(T data, WrapperType &child1, WrapperType &child2, Operation::Operation<T>* op)
      : data(data), op(op), child1(child1), child2(child2) {
    if (child1.get() == child2.get()) {
      throw new std::runtime_error("cannot have the same children for now");
    }
  }

  ScalarValue(T data, WrapperType &child1, WrapperType &child2, Operation::Operation<T>* op,
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
            std::shared_ptr<ScalarValue<T>> &child2, Operation::Operation<T>* op,
            std::string label) {
  return std::shared_ptr<ScalarValue<T>>(
      new ScalarValue<T>(data, child1, child2, op, label));
}
template <typename T>
std::shared_ptr<ScalarValue<T>>
make_scalar(T data, std::shared_ptr<ScalarValue<T>> &child1,
            std::shared_ptr<ScalarValue<T>> &child2, Operation::Operation<T>* op) {
  return std::shared_ptr<ScalarValue<T>>(
      new ScalarValue<T>(data, child1, child2, op));
}


template <typename T>
std::shared_ptr<ScalarValue<T>>
operator+(std::shared_ptr<ScalarValue<T>> left,
          std::shared_ptr<ScalarValue<T>> right) {
  auto op_ptr = Operation::add_ptr<T>;
  return make_scalar(op_ptr->forward(left->data, right->data), left, right, op_ptr);
}

template <typename T>
std::shared_ptr<ScalarValue<T>>
operator+(T left,
          std::shared_ptr<ScalarValue<T>> right) {
  auto left_val = make_scalar(left);
  auto op_ptr = Operation::add_ptr<T>;
  return make_scalar(op_ptr->forward(left_val->data, right->data), left_val, right,
                     op_ptr);
}

template <typename T>
std::shared_ptr<ScalarValue<T>>
operator+(std::shared_ptr<ScalarValue<T>> left,
          T right) {
  auto right_val = make_scalar(right);
  auto op_ptr = Operation::add_ptr<T>;
  return make_scalar(op_ptr->forward(left->data, right_val->data), left,
                     right_val, op_ptr);
}

template <typename T>
std::shared_ptr<ScalarValue<T>>
operator*(std::shared_ptr<ScalarValue<T>> left,
          std::shared_ptr<ScalarValue<T>> right) {
  auto op_ptr = Operation::mul_ptr<T>;
  return make_scalar(op_ptr->forward(left->data, right->data), left, right, op_ptr);
}

template <typename T>
std::shared_ptr<ScalarValue<T>>
operator*(T left,
          std::shared_ptr<ScalarValue<T>> right) {
  auto left_val = make_scalar(left);
  auto op_ptr = Operation::mul_ptr<T>;
  return make_scalar(op_ptr->forward(left_val->data, right->data), left_val, right,
                     op_ptr);
}

template <typename T>
std::shared_ptr<ScalarValue<T>>
operator*(std::shared_ptr<ScalarValue<T>> left,
          T right) {
  auto right_val = make_scalar(right);
  auto op_ptr = Operation::mul_ptr<T>;
  return make_scalar(op_ptr->forward(left->data, right_val->data), left, right_val, op_ptr);
}

template <typename T>
std::shared_ptr<ScalarValue<T>>
pow(std::shared_ptr<ScalarValue<T>> val, T power) {
  std::shared_ptr<ScalarValue<T>> tmp;
  auto op_ptr = Operation::pow_cache<T>(power);
  return make_scalar(op_ptr->forward(val->data, 0), val, tmp, op_ptr);
}
/*
template <typename T>
std::shared_ptr<ScalarValue<T>>
operator/(std::shared_ptr<ScalarValue<T>> num,
          std::shared_ptr<ScalarValue<T>> den) {
  return num * pow(den, -1);
}
*/

template <typename T>
bool operator==(const std::shared_ptr<ScalarValue<T>> &left,
                const std::shared_ptr<ScalarValue<T>> &right) {
  return left->data == right->data;
}

template <typename T>
bool operator==(const std::shared_ptr<ScalarValue<T>> &left,
                const T &right) {
  return left->data == right;
}
template <typename T>
bool operator==(const T &left,
                const std::shared_ptr<ScalarValue<T>> &right) {
  return left == right->data;
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
  auto op_ptr = Operation::tanh_ptr<T>;
  return make_scalar<T>(op_ptr->forward(val->data, 0), val, tmp, op_ptr);
}

template <typename T>
std::shared_ptr<ScalarValue<T>>
exp(std::shared_ptr<ScalarValue<T>> val) {
  std::shared_ptr<ScalarValue<T>> tmp;
  auto op_ptr = Operation::exp_ptr<T>;
  return make_scalar<T>(op_ptr->forward(val->data, 0), val, tmp, Operation::exp_ptr<T>);
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
