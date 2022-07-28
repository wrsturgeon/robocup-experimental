#if UTIL_ENABLED
#ifndef UTIL_CONSTEXPR_MATH_HPP_
#define UTIL_CONSTEXPR_MATH_HPP_

#include <stddef.h>
#include <stdint.h>
#include <type_traits>

namespace util {



// Bit-shifting operator accepting negative shifts.
template <uint8_t bits, typename T>
INLINE constexpr T rshift(T x);



// Log base-2, plus one (floored).
template <typename T = size_t>
INLINE constexpr uint8_t lgp1(T x);



} // namespace util

#endif // UTIL_CONSTEXPR_MATH_HPP_

#else // UTIL_ENABLED
#pragma message("Skipping constexpr_math.hpp; util module disabled")
#endif // UTIL_ENABLED
