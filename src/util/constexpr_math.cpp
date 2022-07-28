#include "util/constexpr_math.hpp"

namespace util {



// Bit-shifting operator accepting negative shifts.
template <uint8_t bits, typename T>
INLINE constexpr T rshift(T x) {
  static_assert(std::is_integral<T>::value, "Can't rshift a non-integral type");
  if constexpr (bits < 0) {
    return x << -bits;
  } else {
    return x >> bits;
  }
}



// Log base-2, plus one (floored).
template <typename T = size_t>
INLINE constexpr uint8_t lgp1(T x) {
  static_assert(std::is_integral<T>::value, "Can't lgp1 a non-integral type");
  return x ? 1 + lgp1(x >> 1) : 0;
}



} // namespace util
