#pragma once

#include <type_traits>

template <typename T_o, typename T_i>
static constexpr auto
isqrt(T_i y) -> T_o {
  static_assert(std::is_integral<T_i>::value, "isqrt works only on integral types");
  static_assert(std::is_integral<T_o>::value, "isqrt works only on integral types");
  static_assert(sizeof(T_i) >= sizeof(T_o), "sizeof isqrt return type must be >= input type");
  T_o x = 0;
  while (x * x < y) { ++x; }
  return x;
}
