#pragma once
#include "scalar.hpp"
#include "operation.hpp"
#include <fmt/format.h>

template <typename T>
struct fmt::formatter<ScalarNS::ScalarValue<T>> : formatter<string_view> {

  template <typename FormatContext>
  auto format(const ScalarNS::ScalarValue<T> &sv, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    // ctx.out() is an output iterator to write to.
    return fmt::format_to(ctx.out(), "{}(data={}, grad={}, op={})", sv.label,
                          sv.data, sv.grad, *sv.op, sv.op->get_type());
  }
};

template <typename T>
struct fmt::formatter<ScalarNS::Scalar<T>> : formatter<string_view> {

  template <typename FormatContext>
  auto format(const ScalarNS::Scalar<T> &sv, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    // ctx.out() is an output iterator to write to.
    return fmt::format_to(ctx.out(), "{}", *sv);
  }
};

template <typename T>
struct fmt::formatter<Operation::Operation<T>> : formatter<string_view> {

  template <typename FormatContext>
  auto format(const Operation::Operation<T> &sv, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", sv.get_symbol());
  }
};

template <> struct fmt::formatter<Operation::OpType> : formatter<string_view> {

  template <typename FormatContext>
  auto format(const Operation::OpType &ot, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    switch (ot) {
    case Operation::OpType::NONE:
      return fmt::format_to(ctx.out(), "NONE");
    case Operation::OpType::UNARY:
      return fmt::format_to(ctx.out(), "UNARY");
    case Operation::OpType::BINARY:
      return fmt::format_to(ctx.out(), "BINARY");
    }
    return ctx.out();
  }
};
