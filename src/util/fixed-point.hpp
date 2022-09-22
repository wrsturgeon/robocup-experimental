#pragma once

#include "util/custom-ints.hpp"
#include "util/uninitialized.hpp"

#include <cmath>
#include <string>

template <std::uint8_t b, std::uint8_t f> class fp {
  static_assert((b & (b - 1)) == 0, "b must be a power of 2");
  static_assert(f < b, "Fractional bits must be < total bits (space for sign bit)");
 private:
  using u8 = std::uint8_t;  // for line-length constraints
 protected:
  using T = typename custom_int<b>::signed_t;
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  T internal;  // NOLINT(misc-non-private-member-variables-in-classes)
  explicit constexpr fp(T x) : internal{x} {}
 public:
  explicit constexpr fp(float x) : internal{static_cast<T>(ldexpf(x, f))} {}
  explicit constexpr fp(double x) : internal{static_cast<T>(ldexp(x, f))} {}
  explicit operator std::string() const { return std::to_string(ldexp(internal, -f)); }
  template <u8 b2, u8 f2> explicit operator fp<b2, f2>() const;
  template <u8 b2, u8 f2> auto operator*(fp<b2, f2> const& x) const -> fp<b, f>;
  auto operator+(fp<b, f> const& x) const -> fp<b, f>;
  auto operator<<(u8 x) const -> fp<b, f> { return fp<b, f>{static_cast<T>(internal << x)}; }
  auto operator>>(u8 x) const -> fp<b, f> { return fp<b, f>{static_cast<T>(internal >> x)}; }
};

template <std::uint8_t b, std::uint8_t f>
template <std::uint8_t b2, std::uint8_t f2>
fp<b, f>::operator fp<b2, f2>() const {
  static_assert((b - f) >= (b2 - f2), "Can't risk integer overflow with fixed-point types");
  static constexpr std::int8_t df = f2 - f;
  if constexpr (df < 0) {
    // Losing precision, but that's generally fine--no possibility of overflow
    return fp<b2, f2>{static_cast<typename fp<b2, f2>::T>(internal >> -df)};
  } else {
    return fp<b2, f2>{static_cast<typename fp<b2, f2>::T>(internal << df)};
  }
}

template <std::uint8_t b, std::uint8_t f>
template <std::uint8_t b2, std::uint8_t f2>
auto
fp<b, f>::operator*(fp<b2, f2> const& x) const -> fp<b, f> {
  return fp<b, f>{static_cast<T>((internal * x.internal) >> f2)};
}

template <std::uint8_t b, std::uint8_t f>
auto
fp<b, f>::operator+(fp<b, f> const& x) const -> fp<b, f> {
  return fp<b, f>{static_cast<T>(internal + x.internal)};
}

template <std::uint8_t b, std::uint8_t f>
auto
operator+(std::string const& s, fp<b, f> const& p) -> std::string {
  return s + static_cast<std::string>(p);
}
