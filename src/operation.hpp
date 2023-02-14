#pragma once
#include <cmath>
#include <fmt/format.h>
#include <stdexcept>
#include <string>
#include <unordered_map>
namespace Operation {

// TODO turn this into a class with forward and backward methods
  enum class OpType { NONE, UNARY, BINARY };

  // unary operators ignore the 2nd argument
  // could replace it with variant, not sure which is better.
  template <typename T>
  struct Operation {
  private:
    static const OpType type = OpType::NONE;
  public:
    virtual const OpType get_type() const
    {
      return type;
    }
    static const std::string symbol;
    virtual const std::string get_symbol() const
    {
      return symbol;
    }

    virtual T forward(T first, T second) {
      throw new std::runtime_error("in Operation forward, not implemented");
    }
    // grad: gradient coming into the op
    // curr_data: "activation" of the current child
    // other_data: "activation" of the other child
    virtual T backward(T grad, T curr_data, T other_data) {
      throw new std::runtime_error("in Operation backward, not implemented");
    }
  };
  template <typename T>
  const std::string Operation<T>::symbol = "";

  template <typename T>
  struct Add : Operation<T> {
  private:
    static const OpType type = OpType::BINARY;
  public:
    const OpType get_type() const { return type; }
    static const std::string symbol;
    const std::string get_symbol() const { return symbol; }
    T forward(T first, T second) { return first + second; }
    T backward(T grad, T curr_data, T other_data) { return grad; }
  };

  template <typename T>
  const std::string Add<T>::symbol = "+";

  template <typename T>
  static Add<T> add_singleton = Add<T>();
  template <typename T>
  static Add<T> *add_ptr = &add_singleton<T>;

  template <typename T>
  struct Mul : Operation<T> {
  private:
    static const OpType type = OpType::BINARY;
  public:
    const OpType get_type() const { return type; }
    static const std::string symbol;
    const std::string get_symbol() const { return symbol; }
    T forward(T first, T second) { return first * second; }
    T backward(T grad, T _, T other_data) { return grad * other_data; }
  };

  template <typename T>
  const std::string Mul<T>::symbol = "*";

  template <typename T>
  static Mul<T> mul_singleton = Mul<T>();
  template <typename T>
  static Mul<T> *mul_ptr = &mul_singleton<T>;

  template <typename T>
  struct Pow : Operation<T> {
  private:
    static const OpType type = OpType::UNARY;
  public:
    const OpType get_type() const { return type; }
    T power;
    Pow(T power) : power(power) {}
    static const std::string symbol;
    const std::string get_symbol() const { return symbol; }
    T forward(T first, T _) {
      return std::pow(first, power);
    }
    T backward(T grad, T curr_data, T _) {
      return power * grad * std::pow(curr_data, power - 1);
    }
  };
  template <typename T>
  const std::string Pow<T>::symbol = "pow";

  template <typename Op, typename T>
  struct OpCache {
    std::unordered_map<T, Op*> cache;
    Op* add(T val) { return cache[val] = new Op(val); }
    Op* get(T val) {
      if (cache.contains(val)) {
        return cache[val];
      }
      return add(val);
    }
    ~OpCache() {
      for (auto i : cache) {
        delete i.second;
      }
    }
  };

  template <typename T>
  static OpCache<Pow<T>, T> pow_cache;

  // TODO add static array of pows for fast access of low powers

  template <typename T>
  struct None : Operation<T> {
  private:
    static const OpType type = OpType::NONE;
  public:
    const OpType get_type() const { return type; }
    static const std::string symbol;
    T forward(T first, T second) { return 137; }
    T backward(T grad, T curr_data, T other_data) { return 137; }
  };

  template <typename T>
  const std::string None<T>::symbol = "";

  template <typename T>
  static None<T> none_singleton = None<T>();
  template <typename T>
  static None<T> *none_ptr = &none_singleton<T>;

  template <typename T> T my_tanh(T in) { return static_cast<T>(std::tanh(in));}
  template <typename T> T my_exp(T in) { return static_cast<T>(std::exp(in)); }

  template <typename T>
  struct Tanh : Operation<T> {
  private:
    static const OpType type = OpType::UNARY;
  public:
    const OpType get_type() const { return type; }
    static const std::string symbol;
    const std::string get_symbol() const { return symbol; }
    T forward(T first, T _) { return my_tanh(first); }
    T backward(T grad, T curr_data, T _) {
      return 1 - std::pow(my_tanh(curr_data), 2) * grad;
    }
  };

  template <typename T>
  const std::string Tanh<T>::symbol = "tanh";

  template <typename T> static Tanh<T> tanh_singleton = Tanh<T>();
  template <typename T> static Tanh<T> *tanh_ptr = &tanh_singleton<T>;

  template <typename T>
  struct Exp : Operation<T> {
  private:
    static const OpType type = OpType::UNARY;
  public:
    const OpType get_type() const { return type; }
    static const std::string symbol;
    const std::string get_symbol() const { return symbol; }
    T forward(T first, T _) { return my_exp(first); }
    T backward(T grad, T curr_data, T _) {
      return my_exp(curr_data) * grad;
    }
  };
  template <typename T>
  const std::string Exp<T>::symbol = "exp";

  template <typename T> static Exp<T> exp_singleton = Exp<T>();
  template <typename T> static Exp<T> *exp_ptr = &exp_singleton<T>;

} // namespace Operation

template <typename T>
struct fmt::formatter<Operation::Operation<T>> : formatter<string_view> {

  template <typename FormatContext>
  auto format(const Operation::Operation<T> &sv,
              FormatContext &ctx) const -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", sv.get_symbol());
  }
};

template <>
struct fmt::formatter<Operation::OpType> : formatter<string_view> {

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
