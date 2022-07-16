#ifndef UTIL_CONSTEXPR_MATH_HPP_
#define UTIL_CONSTEXPR_MATH_HPP_

#include <stdint.h>
#include <type_traits>

#include <util/specifiers.hpp>



// Bit-shifting operator accepting negative shifts.
template <typename T, uint8_t bits>
INLINE constexpr T rshift(T x) {
  if constexpr (bits < 0) {
    return x << -bits;
  } else {
    return x >> bits;
  }
}



// Log base 2.
template <typename T>
constexpr uint8_t lg(T x) {
  static_assert(std::is_integral<T>::value, "T must be an integral type");
  return x ? 1 + lg(x >> 1) : 0;
}



#endif // UTIL_CONSTEXPR_MATH_HPP_