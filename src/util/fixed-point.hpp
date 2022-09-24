#pragma once

#include "util/custom-int.hpp"
#include "util/uninitialized.hpp"

#include <bit>
#include <cassert>
#include <cmath>
#include <string>
#include <type_traits>

#define OTHER_TEMPLATE template <std::uint8_t b2, std::uint8_t f2>
#define INL inline __attribute__((always_inline))

template <std::uint8_t b, std::uint8_t f, typename u = signed> class fp {
  static_assert((b & (b - 1)) == 0, "b must be a power of 2");
  static_assert(f <= b - std::is_signed<u>::value, "Fractional bits > total - ?sign bit");
 protected:
  using T = typename custom_int<b, u>::t;
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  T internal;  // NOLINT(misc-non-private-member-variables-in-classes)
  explicit constexpr fp(T x) : internal{x} {}
 private:
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  using i = typename custom_int<8, u>::t;
 public:
  constexpr explicit fp() : internal{0} {}
  explicit operator std::string() const { return std::to_string(ldexp(internal, -f)); }
  constexpr explicit operator bool() const { return static_cast<bool>(internal); }
  OTHER_TEMPLATE INL constexpr explicit operator fp<b2, f2, u>() const;
  constexpr auto INL operator+(fp<b, f, u> const& x) const -> fp<b, f, u>;
  OTHER_TEMPLATE INL constexpr auto operator*(fp<b2, f2, u> const& x) const -> fp<b, f, u>;
  OTHER_TEMPLATE INL constexpr auto operator/(fp<b2, f2, u> const& x) const -> fp<b, f, u>;
  constexpr auto INL operator+=(fp<b, f, u> const& x) -> fp<b, f, u>&;
  OTHER_TEMPLATE INL constexpr auto operator*=(fp<b2, f2, u> const& x) -> fp<b, f, u>&;
  OTHER_TEMPLATE INL constexpr auto operator/=(fp<b2, f2, u> const& x) -> fp<b, f, u>&;
  constexpr auto INL operator~() const -> fp<b, f, u> { return fp<b, f, u>{~internal}; }
  constexpr auto INL operator<<(i x) const -> fp<b, f, u>;
  constexpr auto INL operator>>(i x) const -> fp<b, f, u>;
  [[nodiscard]] constexpr auto sqrt() const -> fp<b, f, u>;
};

// template <std::uint8_t b, std::uint8_t f, typename u>
// template <std::uint8_t b2, std::uint8_t f2, typename u>
// constexpr fp<b, f, u>::operator fp<b2, f2, u>() const {
//   static_assert((b - f) >= (b2 - f2), "Can't risk integer overflow with fixed-point types");
//   static constexpr std::int8_t df = f2 - f;
//   if constexpr (df < 0) {
//     // Losing precision, but that's generally fine--no possibility of overflow
//     return fp<b2, f2, u>{static_cast<typename fp<b2, f2, u>::T>(internal >> -df)};
//   } else {
//     return fp<b2, f2, u>{static_cast<typename fp<b2, f2, u>::T>(internal << df)};
//   }
// }

template <std::uint8_t b, std::uint8_t f, typename u>
INL constexpr auto
fp<b, f, u>::operator+(fp<b, f, u> const& x) const -> fp<b, f, u> {
  return fp<b, f, u>{static_cast<T>(internal + x.internal)};
}

template <std::uint8_t b, std::uint8_t f, typename u>
template <std::uint8_t b2, std::uint8_t f2>
INL constexpr auto
fp<b, f, u>::operator*(fp<b2, f2, u> const& x) const -> fp<b, f, u> {
  using eb_t = typename custom_int<
        std::bit_ceil(static_cast<std::uint8_t>(b + b2 - std::is_unsigned<u>::value)),
        u>::t;
  return fp<b, f, u>{
        static_cast<T>((static_cast<eb_t>(internal) * static_cast<eb_t>(x.internal)) >> f2)};
}

template <std::uint8_t b, std::uint8_t f, typename u>
template <std::uint8_t b2, std::uint8_t f2>
INL constexpr auto
fp<b, f, u>::operator/(fp<b2, f2, u> const& x) const -> fp<b, f, u> {
  using eb_t = typename custom_int<std::bit_ceil(static_cast<std::uint8_t>(b + f2)), u>::t;
  assert(x);  // no division by zero
  return fp<b, f, u>{static_cast<T>((static_cast<eb_t>(internal) << f2) / x.internal)};
}

template <std::uint8_t b, std::uint8_t f, typename u>
template <std::uint8_t b2, std::uint8_t f2>
INL constexpr auto
fp<b, f, u>::operator*=(fp<b2, f2, u> const& x) -> fp<b, f, u>& {
  using eb_t = typename custom_int<
        std::bit_ceil(static_cast<std::uint8_t>(b + b2 - std::is_unsigned<u>::value)),
        u>::t;
  internal = static_cast<T>(
        (static_cast<eb_t>(internal) * static_cast<eb_t>(x.internal)) >> f2);
  return *this;
}

template <std::uint8_t b, std::uint8_t f, typename u>
template <std::uint8_t b2, std::uint8_t f2>
INL constexpr auto
fp<b, f, u>::operator/=(fp<b2, f2, u> const& x) -> fp<b, f, u>& {
  using eb_t = typename custom_int<std::bit_ceil(static_cast<std::uint8_t>(b + f2)), u>::t;
  assert(x);  // no division by zero
  internal = static_cast<T>((static_cast<eb_t>(internal) << f2) / x.internal);
  return *this;
}

template <std::uint8_t b, std::uint8_t f, typename u>
INL constexpr auto
fp<b, f, u>::operator<<(i x) const -> fp<b, f, u> {
  return fp<b, f, u>{static_cast<T>(internal << x)};
}

template <std::uint8_t b, std::uint8_t f, typename u>
INL constexpr auto
fp<b, f, u>::operator>>(i x) const -> fp<b, f, u> {
  return fp<b, f, u>{static_cast<T>(internal >> x)};
}

template <std::uint8_t b, std::uint8_t f, typename u>
auto
operator+(std::string const& s, fp<b, f, u> const& p) -> std::string {
  return s + static_cast<std::string>(p);
}

template <std::uint8_t b, std::uint8_t f, typename u>
auto
operator<<(std::ostream& os, fp<b, f, u> const& p) -> std::ostream& {
  return os << static_cast<std::string>(p);
}

template <std::uint8_t b, std::uint8_t f, typename u>
constexpr auto
fp<b, f, u>::sqrt() const -> fp<b, f, u> {
  // Babylonian method
  constexpr std::uint8_t n_times = std::bit_width(b);
  if (!*this) { return fp<b, f, u>{/* 0 */}; }
  fp<b, f, u> x = *this;
  for (std::uint8_t i = 0; i < n_times; ++i) { x = (x + (*this / x)) >> 1; }
  return x;
}

#undef INL
#undef OTHER_TEMPLATE
