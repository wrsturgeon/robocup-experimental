#pragma once

#include <cstdint>
#include <type_traits>

template <typename T_o, typename T_i>
constexpr auto
rshift(T_i x, std::int8_t b) -> T_o {
  // fuck performance, make it pretty <3 (constexpr so doesn't matter)
  bool const n = x < 0;
  auto y = static_cast<typename std::make_unsigned<T_i>::type>(n ? -x : x);
  auto z = static_cast<T_o>((b < 0) ? (y << -b) : (y >> b));
  return n ? -z : z;
}

template <typename T_o, typename T_i>
constexpr auto
lshift(T_i x, std::int8_t b) -> T_o {
  return rshift<T_o>(x, -b);
}
