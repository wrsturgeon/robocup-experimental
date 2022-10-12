#pragma once

#include <type_traits>

// DO NOT USE THIS IN ACTUAL NON-CONSTEXPR CODE
template <typename T_o, typename T_i>
[[nodiscard]] static constexpr auto isqrt(T_i y) -> T_o {
  static_assert(std::is_integral_v<T_i>, "isqrt works only on integral types");
  static_assert(std::is_integral_v<T_o>, "isqrt works only on integral types");
  static_assert(sizeof(T_i) >= sizeof(T_o), "sizeof isqrt return type must be >= input type");
  T_o x = 0;
  while (x * x < y) {
    ++x;
  }
  return x;
}
