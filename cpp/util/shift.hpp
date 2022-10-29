#pragma once

#include "util/ints.hpp"

// Copied from fixed-point.hpp (TODO(wrsturgeon): better solution)
namespace fp {
template <u8 B, i8 I, typename S>
class t;
}  // namespace fp

static constexpr i8 kMostNegative = -128;

template <i8 b, typename T>
requires (b != kMostNegative)
pure auto lshift(T x) noexcept -> T;
template <i8 b, typename T>
requires (b != kMostNegative)
pure auto rshift(T x) noexcept -> T;

template <i8 b, typename T>
requires (b != kMostNegative)
pure auto lshift(T x) noexcept -> T {
  if constexpr (b == 0) { return x; }
  if constexpr (b < 0) { return rshift<-b>(x); }
  return static_cast<T>(x << b);
}

template <i8 b, typename T>
requires (b != kMostNegative)
pure auto rshift(T x) noexcept -> T {
  if constexpr (b == 0) { return x; }
  if constexpr (b < 0) { return lshift<-b>(x); }
  return static_cast<T>(x >> b);
}

template <i8 b, u8 B, i8 I, typename S>
pure auto
lshift(fp::t<B, I, S> x) noexcept -> fp::t<B, I + b, S> {
  return fp::t<B, I + b, S>{+x};
}
template <i8 b, u8 B, i8 I, typename S>
pure auto
rshift(fp::t<B, I, S> x) noexcept -> fp::t<B, I - b, S> {
  return fp::t<B, I - b, S>{+x};
}
