#include <fmt/format.h>
#include <string>

enum class Operation { None, Add, Mul };

template <> struct fmt::formatter<Operation> : formatter<string_view> {

  template <typename FormatContext>
  auto format(const Operation &op, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    // ctx.out() is an output iterator to write to.
    switch (op) {
    case Operation::Add:
      return fmt::format_to(ctx.out(), "+");
    case Operation::Mul:
      return fmt::format_to(ctx.out(), "*");
    case Operation::None:
      return fmt::format_to(ctx.out(), "None");
    }
    return ctx.out();
  }
};
