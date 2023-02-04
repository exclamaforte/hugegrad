#include <fmt/format.h>
#include <string>

// just copying Karpathy for now, expand to full tensors eventually
template <typename T>
struct ScalarValue
{
  friend class fmt::formatter<ScalarValue<T>>;

  template<typename K> friend class ScalarValue;

  ScalarValue(T data) : data(data) {}
  ScalarValue() : data(0) {}

  template<typename K>
  ScalarValue<T> operator+(const ScalarValue<K>& other)
  {
    ScalarValue<T> result(data + other.data);
    return result;
  }

  template<typename K>
  bool operator==(const ScalarValue<K>& other) const
  {
    return data == other.data;
  }

  template <typename K>
  bool operator!=(const ScalarValue<K> &other) const {
    return data != other.data;
  }

private:
  T data;
};

template <typename T>
struct fmt::formatter<ScalarValue<T>> : formatter<string_view> {

  template <typename FormatContext>
  auto format(const ScalarValue<T> &sv, FormatContext &ctx) const -> decltype(ctx.out()) {
    // ctx.out() is an output iterator to write to.
    return fmt::format_to(ctx.out(), "Value(data={})", sv.data);
  }
};
