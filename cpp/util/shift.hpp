#pragma once

#include <cstdint>

static constexpr std::int8_t kMostNegative = -128;

template <std::int8_t b, typename T> requires (b != kMostNegative)
pure auto lshift(T x) noexcept -> T;
template <std::int8_t b, typename T> requires (b != kMostNegative)
pure auto rshift(T x) noexcept -> T;

template <std::int8_t b, typename T> requires (b != kMostNegative)
pure auto lshift(T x) noexcept -> T {
  if constexpr (b == 0) { return x; }
  if constexpr (b < 0) { return rshift<-b>(x); }
  return static_cast<T>(x << b);
}

template <std::int8_t b, typename T> requires (b != kMostNegative)
pure auto rshift(T x) noexcept -> T {
  if constexpr (b == 0) { return x; }
  if constexpr (b < 0) { return lshift<-b>(x); }
  return static_cast<T>(x >> b);
}
