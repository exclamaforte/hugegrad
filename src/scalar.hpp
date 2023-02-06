#include <fmt/format.h>
#include "operation.hpp"
#include <string>
#include <optional>
#include <memory>
#include <tuple>
#define FMT_ENFORCE_COMPILE_STRING 1

template<typename T> struct ScalarValue;


template <typename T>
struct ScalarValue
{
  using WrapperType = std::shared_ptr<ScalarValue<T>>;



  WrapperType child1;
  WrapperType child2;
  Operation op;
  T data;
  T grad;
  std::string label;

  ScalarValue(T data) : data(data), op(Operation::None) {}

  ScalarValue(T data, WrapperType &child1, WrapperType &child2, Operation op, std::string& label)
    : data(data), op(op), child1(child1), child2(child2), label(label) {}
  ScalarValue() = default;
};

template <typename T>
std::shared_ptr<ScalarValue<T>> make_scalar(T data)
{
  return std::shared_ptr<ScalarValue<T>>(new ScalarValue<T>(data));
}

template <typename T>
std::shared_ptr<ScalarValue<T>> make_scalar(T data, std::shared_ptr<ScalarValue<T>> &child1,
                                            std::shared_ptr<ScalarValue<T>> &child2,
                                            Operation op, std::string &label) {
  return std::shared_ptr<ScalarValue<T>>(new ScalarValue<T>(data, child1, child2, op, label));
}

template <typename T>
struct fmt::formatter<ScalarValue<T>> : formatter<string_view> {

  template <typename FormatContext>
  auto format(const ScalarValue<T> &sv, FormatContext &ctx) const -> decltype(ctx.out()) {
    // ctx.out() is an output iterator to write to.
    return fmt::format_to(ctx.out(), "Value(data={})", sv.data);
  }
};

template <typename T>
struct fmt::formatter<std::shared_ptr<ScalarValue<T>>> : formatter<string_view> {

  template <typename FormatContext>
  auto format(const std::shared_ptr<ScalarValue<T>> &sv, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    // ctx.out() is an output iterator to write to.
    return fmt::format_to(ctx.out(), "{}", *sv);
  }
};

// template <typename K, typename T>
// std::shared_ptr<ScalarValue<K>> operator+(std::shared_ptr<ScalarValue<K>> &left, std::shared_ptr<ScalarValue<T>> &right) {
//   K data = left->data + right->data;
//   return make_scalar(data, left, right, Operation::Add);
// }

template <typename T>
std::shared_ptr<ScalarValue<T>>
operator+(std::shared_ptr<ScalarValue<T>> left,
          std::shared_ptr<ScalarValue<T>> right) {
  T data = left->data + right->data;
  return make_scalar(data, left, right, Operation::Add);
}

template <typename K, typename T>
std::shared_ptr<ScalarValue<K>> operator*(std::shared_ptr<ScalarValue<K>> left, std::shared_ptr<ScalarValue<T>> right) {
  K data = left->data * right->data;
  return make_scalar(data, left, right, Operation::Add);
}

template <typename K, typename T>
bool operator==(const std::shared_ptr<ScalarValue<K>> left, const std::shared_ptr<ScalarValue<T>> right) {
  return left->data == right->data;
}

template <typename K, typename T>
bool operator!=(const std::shared_ptr<ScalarValue<K>> left, const std::shared_ptr<ScalarValue<T>> right) {
  return left->data != right->data;
}

template <typename T>
void rec_helper(std::shared_ptr<ScalarValue<T>> *val, void *parent,
                std::back_insert_iterator<std::string> c) {
  void *val_void = static_cast<void *>(val);
  fmt::format_to(c, "id{} [label=\"{} | {} | {}\"]\n", val_void, val->label, val->data, val->grad);
  if (parent) {
    fmt::format_to(c, "id{} -> id{}\n", val_void, parent);
  }
  if (val->op != Operation::None) {
    void *op_id = &val->op;
    fmt::format_to(c, "id{} [label='{}']\n id{} -> id{}\n", op_id, val->op,
                   op_id, val_void);
    if (val->child1) {
      rec_helper(val->child1, op_id, c);
    }
    if (val->child2) {
      rec_helper(val->child2, op_id, c);
    }
  }
}

template <typename T>
std::string gen_vis(std::shared_ptr<ScalarValue<T>> vis) {
  std::string result = "";
  rec_helper(vis, nullptr, std::back_inserter(result));
  return fmt::format("{}", result);
}
