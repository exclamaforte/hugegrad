#pragma once
#include <functional>
// epsilons set to balance error in floating point math with
// better estimates coming from being as small as possible
template <typename T> constexpr T epsilon;

template <> constexpr float epsilon<float> = 0.001f;
template <> constexpr double epsilon<double> = 0.001;


//calculate using the symmetric difference quotient
template <typename T>
std::function<T(T)> derivative(std::function<T(T)> f)
{
  return [f](T x) {
    constexpr T h = epsilon<T>;
    return (f(x + h) - f(x - h)) / (2 * h);
  };
}
