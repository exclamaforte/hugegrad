#pragma once
#include "operation.hpp"
#include <fmt/format.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <cmath>
#include <type_traits>
namespace ScalarNS {

template <typename T> struct ScalarValue;

template <typename T> using Scalar = std::shared_ptr<ScalarValue<T>>;

enum class SeenMark { NONE, TMP, PERM };
template <typename T>
struct ScalarValue {
  Scalar<T> child1;
  Scalar<T> child2;

  Operation::Operation<T>* op;

  // aka "activation"
  T data = 0;
  // aka "gradient"
  T grad = 0;

  std::string label;

  SeenMark seen = SeenMark::NONE;

  void clear_gradient() {
    grad = 0;
    seen = false;
    if (op->type != Operation::OpType::NONE) {
      child1->clear_gradient();
      child2->clear_gradient();
    }
  }

  // derivative of anything with respect to itself is 1
  void compute_grad(T prev_grad = 1) {
    //fmt::print("{}\n", *this);
    if (seen != SeenMark::NONE) {
      //fmt::print("throwing\n");
      throw new std::runtime_error(fmt::format("node {} has been seen before, aborting", label));
    }
    seen = SeenMark::PERM;
    grad += prev_grad;
    switch (op->get_type()) {
    case Operation::OpType::BINARY:
      child1->compute_grad(op->backward(grad, child1->data, child2->data));
      child2->compute_grad(op->backward(grad, child2->data, child1->data));
      break;
    case Operation::OpType::UNARY:
      child1->compute_grad(op->backward(grad, child1->data, 0));
      break;
    case Operation::OpType::NONE:
      break;
    }
  }

  void propagate_gradient() {
    // assumes grad is set to the correct value
    // used to propagate gradients from topolgical sort
    switch (op->get_type()) {
    case Operation::OpType::BINARY:
      child1->grad += op->backward(grad, child1->data, child2->data);
      child2->grad += op->backward(grad, child2->data, child1->data);
      break;
    case Operation::OpType::UNARY:
      child1->grad += op->backward(grad, child1->data, 0);
      break;
    case Operation::OpType::NONE:
      break;
    }
  }

  ScalarValue(T data) : data(data), op(Operation::none_ptr<T>) {}
  ScalarValue(T data, std::string label) : data(data), label(label), op(Operation::none_ptr<T>) {}

  ScalarValue(T data, Scalar<T> &child1, Scalar<T> &child2, Operation::Operation<T>* op)
      : data(data), op(op), child1(child1), child2(child2) {
    if (child1.get() == child2.get()) {
      throw new std::runtime_error("cannot have the same children for now");
    }
  }

  ScalarValue(T data, Scalar<T> &child1, Scalar<T> &child2, Operation::Operation<T>* op,
              std::string &label)
      : data(data), op(op), child1(child1), child2(child2), label(label) {
    if (child1.get() == child2.get()) {
      throw new std::runtime_error("cannot have the same children for now");
    }
  }
  ScalarValue() = default;
};

template <typename T> Scalar<T> make_scalar(T data) {
  return Scalar<T>(new ScalarValue<T>(data));
}

template <typename T> Scalar<T> make_scalar(T data, std::string label) {
  return Scalar<T>(new ScalarValue<T>(data, label));
}

template <typename T>
Scalar<T> make_scalar(T data, Scalar<T> &child1,
                      Scalar<T> &child2, Operation::Operation<T>* op,
                      std::string label) {
  return Scalar<T>(new ScalarValue<T>(data, child1, child2, op, label));
}
template <typename T>
Scalar<T> make_scalar(T data, Scalar<T> &child1, Scalar<T> &child2,
                      Operation::Operation<T>* op) {
  return Scalar<T>(new ScalarValue<T>(data, child1, child2, op));
}

template <typename T>
concept arithmetic = std::integral<T> || std::floating_point<T>;

template <typename T>
Scalar<T> operator+(Scalar<T> left, Scalar<T> right) {
  auto op_ptr = Operation::add_ptr<T>;
  return make_scalar(op_ptr->forward(left->data, right->data), left, right, op_ptr);
}

template <typename T, arithmetic K>
Scalar<T> operator+(K left, Scalar<T> right) {
  auto left_val = make_scalar(static_cast<T>(left));
  auto op_ptr = Operation::add_ptr<T>;
  return make_scalar(op_ptr->forward(left_val->data, right->data), left_val, right,
                     op_ptr);
}

template <typename T, arithmetic K>
Scalar<T> operator+(Scalar<T> left, K right) {
  auto right_val = make_scalar(static_cast<T>(right));
  auto op_ptr = Operation::add_ptr<T>;
  return make_scalar(op_ptr->forward(left->data, right_val->data), left,
                     right_val, op_ptr);
}

template <typename T>
Scalar<T> operator-(Scalar<T> left) {
  return left * -1;
}

template <typename T>
Scalar<T> operator-(Scalar<T> left, Scalar<T> right) {
  return left + -right;
}

template <typename T, arithmetic K>
Scalar<T> operator-(K left, Scalar<T> right) {
  return left + -right;
}

template <typename T, arithmetic K>
Scalar<T> operator-(Scalar<T> left, K right) {
  return left + -right;
}

template <typename T>
Scalar<T> operator*(Scalar<T> left, Scalar<T> right) {
  auto op_ptr = Operation::mul_ptr<T>;
  return make_scalar(op_ptr->forward(left->data, right->data), left, right, op_ptr);
}

template <typename T, arithmetic K>
Scalar<T> operator*(K left, Scalar<T> right) {
  auto left_val = make_scalar(static_cast<K>(left));
  auto op_ptr = Operation::mul_ptr<T>;
  return make_scalar(op_ptr->forward(left_val->data, right->data), left_val, right,
                     op_ptr);
}


template <typename T, arithmetic K>
Scalar<T> operator*(Scalar<T> left, K right) {
  auto right_val = make_scalar(static_cast<T>(right));
  auto op_ptr = Operation::mul_ptr<T>;
  return make_scalar(op_ptr->forward(left->data, right_val->data), left, right_val, op_ptr);
}

template <typename T>
Scalar<T> pow(Scalar<T> val, T power) {
  Scalar<T> tmp;
  auto op_ptr = Operation::pow_cache<T>.get(power);
  return make_scalar(op_ptr->forward(val->data, 0), val, tmp, op_ptr);
}

template <std::floating_point T>
Scalar<T> operator/(Scalar<T> num, Scalar<T> den) {
  return num * pow(den, static_cast<T>(-1.0));
}

template <typename T, typename K>
bool operator==(const Scalar<T> &left,
                const Scalar<K> &right) {
  return left->data == right->data;
}

template <typename T>
bool operator==(const Scalar<T> &left,
                const T &right) {
  return left->data == right;
}
template <typename T>
bool operator==(const T &left,
                const Scalar<T> &right) {
  return left == right->data;
}
template <typename T, typename K>
bool operator!=(const Scalar<T> &left,
                const Scalar<K> &right) {
  return left->data != right->data;
}

template <typename T>
Scalar<T> tanh(Scalar<T> val) {
  Scalar<T> tmp;
  auto op_ptr = Operation::tanh_ptr<T>;
  return make_scalar<T>(op_ptr->forward(val->data, 0), val, tmp, op_ptr);
}

template <std::floating_point T>
Scalar<T> tanh_exp(Scalar<T> val) {
  return (exp(static_cast<T>(2.0) * val) - static_cast<T>(1.0)) /
         (exp(static_cast<T>(2.0) * val) + static_cast<T>(1.0));
}

template <typename T>
Scalar<T> exp(Scalar<T> val) {
  Scalar<T> tmp;
  auto op_ptr = Operation::exp_ptr<T>;
  return make_scalar<T>(op_ptr->forward(val->data, 0), val, tmp, op_ptr);
}
} // namespace Scalar
