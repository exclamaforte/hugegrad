#pragma once
#include <cmath>
#include <fmt/format.h>
#include <string>
namespace Operation {

// TODO turn this into a class with forward and backward methods
  enum class OpType { None, Add, Mul, Tanh, Exp, Pow, Div};

template <typename T> T my_tanh(T in) { return static_cast<T>(std::tanh(in)); }
template <typename T> T my_exp(T in) { return static_cast<T>(std::exp(in)); }

} // namespace Operation

template <> struct fmt::formatter<Operation::OpType> : formatter<string_view> {

  template <typename FormatContext>
  auto format(const Operation::OpType &op, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    // ctx.out() is an output iterator to write to.
    switch (op) {
    case Operation::OpType::Add:
      return fmt::format_to(ctx.out(), "+");
    case Operation::OpType::Mul:
      return fmt::format_to(ctx.out(), "*");
    case Operation::OpType::Tanh:
      return fmt::format_to(ctx.out(), "tanh");
    case Operation::OpType::None:
      return fmt::format_to(ctx.out(), "None");
    }
    return ctx.out();
  }
};
