#pragma once

#include "util/array-inits.hpp"
#include "util/fixed-string.hpp"
#include "util/shift.hpp"
#include "util/ternary.hpp"

#include <array>
#include <compare>
#include <limits>

#ifndef NDEBUG
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#endif

// Forward declarations

namespace fp {
template <u8 B, i8 I, typename S>
requires (B <= kSystemBits)
class t;
template <ufull_t N, u8 B, i8 I, typename S>
requires (B <= kSystemBits)
class a;
}  // namespace fp

template <typename T>
struct is_fixed_point_s {
  static constexpr bool value = false;
};
template <u8 B, i8 I, typename S>
requires (B <= kSystemBits)
struct is_fixed_point_s<fp::t<B, I, S>> {
  static constexpr bool value = true;
};
template <ufull_t N, u8 B, i8 I, typename S>
requires (B <= kSystemBits)
struct is_fixed_point_s<fp::a<N, B, I, S>> {
  static constexpr bool value = true;
};
template <typename T>
inline constexpr bool is_fixed_point = is_fixed_point_s<std::decay_t<T>>::value;
template <typename T> concept FixedPoint = is_fixed_point<T>;

template <typename T>
struct is_fixed_point_array_s {
  static constexpr bool value = false;
};
template <ufull_t N, u8 B, i8 I, typename S>
requires (B <= kSystemBits)
struct is_fixed_point_array_s<fp::a<N, B, I, S>> {
  static constexpr bool value = true;
};
template <typename T>
inline constexpr bool is_fixed_point_array = is_fixed_point_array_s<std::decay_t<T>>::value;
template <typename T> concept FixedPointArray = is_fixed_point_array<T>;

namespace fp {

inline constexpr u8 kCompactBits = 16;  // very arbitrary: enough to have reasonable precision
// also must be less than half system bits to prevent multiplication overflow

template <u8 B1, i8 I1, typename S1, u8 B2, i8 I2, typename S2>
using mul_rtn_t = t<B1 + B2, I1 + I2, std::conditional_t<std::is_signed_v<S1> or std::is_signed_v<S2>, signed, unsigned>>;

// TODO(wrsturgeon): no particular reason `I` can't be negative
template <u8 B, i8 I, typename S>
requires (B <= kSystemBits)
class t {
 public:
  static constexpr u8 b = B;
  static constexpr i8 i = I;
  static constexpr bool s = std::is_signed_v<S>;
  static constexpr i8 f = b - i - s;
  using internal_t = cint<byte_ceil<B>, S>;
  using floor_t = cint<byte_ceil<(i + s < 0) ? 0 : static_cast<u8>(i + s)>, S>;
  using self_t = t<B, I, S>;
  using signed_t = S;
 private:
  internal_t internal;
 public:
  constexpr explicit t(internal_t x) : internal{x} {}
  template <u8 B2, i8 I2, typename S2>
  requires (B2 <= kSystemBits)
  pure explicit operator t<B2, I2, S2>() const NOX;
  template <u8 B2, i8 I2, typename S2>
  requires (B2 <= kSystemBits)
  constexpr explicit t(t<B2, I2, S2> x) : internal{x.operator self_t().internal} {}
  pure static auto max() -> self_t { return self_t{std::numeric_limits<internal_t>::max()}; }
  pure static auto zero() -> self_t { return self_t{0}; }
  template <i8 p>
  requires (p < i)  // and (f + p >= 0))
  pure static auto p2() -> self_t {
    return self_t(lshift<f + p, ufull_t>(1));
  }
  pure static auto unit() -> self_t { return p2<0>(); }
  // pure auto operator+() const noexcept -> internal_t { return internal; }
  pure explicit operator internal_t() const noexcept { return internal; }
  pure auto abs() const noexcept -> t<B, I, unsigned> { return t<B, I, unsigned>{static_cast<cint<B, unsigned>>(std::abs(internal))}; }
  pure auto floor() const noexcept -> floor_t /* clang-format off */ requires (f != 0) { return static_cast<floor_t>(rshift<f, full_t<S>>(internal)); }                                                                                                /* clang-format on */
  pure auto floor() const noexcept -> u8 requires (i + s < 0) /* clang-format off */ { return 0; } /* clang-format on */
  pure auto to_int() const noexcept -> internal_t /* clang-format off */ requires (f == 0) { return internal; }                                                                                                /* clang-format on */
  // pure auto round() const noexcept -> floor_t { return (p2<-1>() + *this).floor(); }
  pure auto squared() const noexcept -> self_t { return (*this) * (*this); }
  INLINE auto operator+=(FixedPoint auto&& x) noexcept -> self_t& { return *this = *this + std::forward<decltype(x)>(x); }
  INLINE auto operator-=(FixedPoint auto&& x) noexcept -> self_t& { return *this = *this - std::forward<decltype(x)>(x); }
  INLINE auto operator*=(FixedPoint auto&& x) noexcept -> self_t& { return *this = *this * std::forward<decltype(x)>(x); }
  INLINE auto operator/=(FixedPoint auto&& x) noexcept -> self_t& { return *this = *this / std::forward<decltype(x)>(x); }
  pure auto operator-() const noexcept -> self_t requires (s) /* clang-format off */ { return self_t{static_cast<internal_t>(-internal)}; } /* clang-format on */
#ifndef NDEBUG
  pure explicit operator std::string() const { return std::to_string(ldexp(internal, -f)); }
#endif
 private:  // Friend declarations
  template <ufull_t N2, u8 B2, i8 I2, typename S2>
  requires (B2 <= kSystemBits)
  friend class a;
  template <u8 B1, i8 I1, typename S1, typename S2>
  requires (B1 <= kSystemBits)
  friend PURE_NOATTR auto operator+(t<B1, I1, S1> const&, t<B1, I1, S2> const&) NOX->t<B1, I1, S1>;
  template <u8 B1, i8 I1, typename S1, typename S2>
  requires (B1 <= kSystemBits)
  friend PURE_NOATTR auto operator-(t<B1, I1, S1> const&, t<B1, I1, S2> const&) NOX->t<B1, I1, S1>;
  friend PURE_NOATTR auto operator==(t<B, I, S> const&, t<B, I, S> const&) noexcept -> bool;
  template <u8 B2, i8 I2, typename S2, u8 B_in, i8 I_in, typename S_in>
  requires ((B2 <= kSystemBits) and (B_in <= kSystemBits))
  friend PURE_NOATTR auto unsafe_cast(t<B_in, I_in, S_in> const& x) noexcept -> t<B2, I2, S2>;
  template <i8 b2, u8 B2, i8 I2, typename S2>
  friend PURE_NOATTR auto ::lshift(fp::t<B2, I2, S2> x) noexcept -> fp::t<B2, I2 + b2, S2>;
  template <i8 b2, u8 B2, i8 I2, typename S2>
  friend PURE_NOATTR auto ::rshift(fp::t<B2, I2, S2> x) noexcept -> fp::t<B2, I2 - b2, S2>;
  template <u8 B1, i8 I1, typename S1, u8 B2, i8 I2, typename S2>
  requires ((B1 <= kSystemBits) and (B2 <= kSystemBits))
  friend PURE_NOATTR auto operator*(t<B1, I1, S1> const& x, t<B2, I2, S2> const& z) noexcept -> mul_rtn_t<B1, I1, S1, B2, I2, S2>;
};

template <ufull_t N, u8 B, i8 I, typename S>
requires (B <= kSystemBits)
class a {
 public:
  static constexpr ufull_t n = N;
  static constexpr u8 b = B;
  static constexpr i8 i = I;
  static constexpr bool s = std::is_signed_v<S>;
  static constexpr i8 f = b - i - s;
  using scalar_t = t<B, I, S>;
  using internal_t = typename scalar_t::internal_t;
  using floor_t = typename scalar_t::floor_t;
  using arr_t = std::array<internal_t, N>;
  using self_t = a<N, B, I, S>;
 private:
  arr_t internal;
  template <ufull_t N2, u8 B2, i8 I2, typename S2>
  requires (B2 <= kSystemBits)
  friend class a;
 public:
  // constexpr explicit a(arr_t const& x) : internal{x} {}
  // constexpr explicit a(arr_t&& x) : internal{std::move(x)} {}
  // template <typename... T> constexpr explicit a(T&&... x) : internal{static_cast<internal_t>(std::forward<T>(x))...} {}
  template <FixedPoint... T>
  requires (std::same_as<scalar_t, std::decay_t<T>> and ...)
  constexpr explicit a(T&&... x) : internal{std::forward<T>(x).internal...} {}
  // constexpr explicit a(T&&... x) : internal{+static_cast<scalar_t>(std::forward<T>(x))...} {}
  pure static auto zero() -> self_t { return self_t{zeros<internal_t, N>()}; }
  // pure auto operator+() const noexcept -> arr_t { return internal; }
  pure auto operator[](ufull_t idx) const -> scalar_t { return scalar_t{internal[idx]}; }
  pure auto begin() const noexcept -> decltype(auto) { return internal.begin(); }
  pure auto end() const noexcept -> decltype(auto) { return internal.end(); }
  pure auto floor() const noexcept -> std::array<floor_t, N>;
  // pure auto round() const noexcept -> std::array<floor_t, N>;
};

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert(sizeof(t<8, 0, signed>) == sizeof(typename t<8, 0, signed>::internal_t));
static_assert(sizeof(a<1, 8, 0, signed>) == sizeof(typename a<1, 8, 0, signed>::internal_t));
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

pure auto
from_int(std::integral auto x) noexcept -> fp::t<kSystemBits, kSystemBits - 1, signed> {
  return fp::t<kSystemBits, kSystemBits - 1, signed>{x};
}
pure auto
from_int(std::unsigned_integral auto x) noexcept -> fp::t<kSystemBits, kSystemBits, unsigned> {
  return fp::t<kSystemBits, kSystemBits, unsigned>{x};
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-variable"
// NOLINTBEGIN(cppcoreguidelines-macro-usage)

template <u8 B, i8 I, typename S, u8 B_in, i8 I_in, typename S_in>
requires ((B <= kSystemBits) and (B_in <= kSystemBits))
pure auto unsafe_cast(t<B_in, I_in, S_in> const& x) noexcept -> t<B, I, S> {
  using in_t = t<B_in, I_in, S_in>;
  using rtn_t = t<B, I, S>;
  return rtn_t{static_cast<typename rtn_t::internal_t>(lshift<rtn_t::f - in_t::f>(static_cast<full_t<S>>(x.internal)))};
}

template <u8 B, i8 I, typename S>
requires (B <= kSystemBits)
template <u8 B2, i8 I2, typename S2>
requires (B2 <= kSystemBits)
pure t<B, I, S>::operator t<B2, I2, S2>() const NOX {
#ifndef NDEBUG
  if ((internal < 0) and std::is_unsigned_v<S2>) { throw std::out_of_range{"Coercing a negative fixed-point integer into an unsigned format"}; }
  if constexpr (I2 < I) {
    // TODO(wrsturgeon): check if we'll have runtime overflow
  }
#endif
  return unsafe_cast<B2, I2, S2>(*this);
}

/******** Mathematical operators ********/

template <u8 B, i8 I, typename S1, typename S2>
requires (B <= kSystemBits)
pure auto
operator+(t<B, I, S1> const& x, t<B, I, S2> const& z) NOX->t<B, I, S1> {
  using rtn_t = t<B, I, S1>;
#ifndef NDEBUG
  if (((x.internal < 0) == (z.internal < 0)) and (static_cast<typename rtn_t::internal_t>(x.internal + z.internal) < 0) != (x.internal < 0)) {
    throw std::overflow_error{
          "Overflow in fp::t<" + std::to_string(B) + ", " + std::to_string(I) + ", " + (std::is_signed_v<S1> ? "signed" : "unsigned") + "> + fp::t<" + std::to_string(B) + ", " + std::to_string(I) + ", " + (std::is_signed_v<S2> ? "signed" : "unsigned") +
          ">: " + static_cast<std::string>(x) + " + " + static_cast<std::string>(z) + " overflows to " + static_cast<std::string>(rtn_t{static_cast<typename rtn_t::internal_t>(x.internal + z.internal)})};
  }
#endif
  return rtn_t{x.internal + z.internal};
}
template <u8 B1, i8 I1, typename S1, u8 B2, i8 I2, typename S2>
requires ((B1 <= kSystemBits) and (B2 <= kSystemBits))
pure auto
operator+(t<B1, I1, S1> const& x, t<B2, I2, S2> const& z) NOX->t<B1, I1, S1> {
  return operator+<B1, I1, S1, S2>(x, static_cast<t<B1, I1, S2>>(z));
}

template <u8 B, i8 I, typename S1, typename S2>
requires (B <= kSystemBits)
pure auto
operator-(t<B, I, S1> const& x, t<B, I, S2> const& z) NOX->t<B, I, S1> {
  using rtn_t = t<B, I, S1>;
#ifndef NDEBUG
  if (((x.internal < 0) != (z.internal < 0)) and (static_cast<typename rtn_t::internal_t>(x.internal - z.internal) < 0) != (x.internal < 0)) {
    throw std::overflow_error{
          "Overflow in fp::t<" + std::to_string(B) + ", " + std::to_string(I) + ", " + (std::is_signed_v<S1> ? "signed" : "unsigned") + "> - fp::t<" + std::to_string(B) + ", " + std::to_string(I) + ", " + (std::is_signed_v<S2> ? "signed" : "unsigned") +
          ">: " + static_cast<std::string>(x) + " - " + static_cast<std::string>(z) + " overflows to " + static_cast<std::string>(rtn_t{static_cast<typename rtn_t::internal_t>(x.internal - z.internal)})};
  }
#endif
  return rtn_t{static_cast<typename rtn_t::internal_t>(x.internal - z.internal)};
}
template <u8 B1, i8 I1, typename S1, u8 B2, i8 I2, typename S2>
requires ((B1 <= kSystemBits) and (B2 <= kSystemBits))
pure auto
operator-(t<B1, I1, S1> const& x, t<B2, I2, S2> const& z) NOX->t<B1, I1, S1> {
  return operator-<B1, I1, S1, S2>(x, static_cast<t<B1, I1, S2>>(z));
}

template <u8 B1, i8 I1, typename S1, u8 B2, i8 I2, typename S2>
requires ((B1 <= kSystemBits) and (B2 <= kSystemBits))
pure auto
operator*(t<B1, I1, S1> const& x, t<B2, I2, S2> const& z) noexcept -> mul_rtn_t<B1, I1, S1, B2, I2, S2> {
  return mul_rtn_t<B1, I1, S1, B2, I2, S2>{x.internal * z.internal};
}

template <u8 B3, i8 I3, typename S3, u8 B1, i8 I1, typename S1, u8 B2, i8 I2, typename S2>
requires ((B1 <= kSystemBits) and (B2 <= kSystemBits) and (B3 <= kSystemBits))
pure auto div(t<B1, I1, S1> const& x, t<B2, I2, S2> const& z) noexcept -> t<B3, I3, S3> {
  using x_t = t<B1, I1, S1>;
  using z_t = t<B2, I2, S2>;
  using rtn_t = t<B3, I3, S3>;
  // if input type = output type, we'd left-shift the numerator by z's fractional bits ... generalize it
  return rtn_t{static_cast<typename rtn_t::internal_t>(lshift<rtn_t::f + z_t::f - x_t::f>(x.internal) / z.internal)};
}
template <u8 B3, i8 I3, typename S3, u8 B1, i8 I1, typename S1, u8 B2, typename S2>
requires ((B1 <= kSystemBits) and (B2 <= kSystemBits) and (B3 <= kSystemBits))
pure auto div(t<B1, I1, S1> const& x, cint<B2, S2> const& z) noexcept -> t<B3, I3, S3> {
  using x_t = t<B1, I1, S1>;
  using rtn_t = t<B3, I3, S3>;
  // if input type = output type, we'd left-shift the numerator by z's fractional bits ... generalize it
  return rtn_t{static_cast<typename rtn_t::internal_t>(lshift<rtn_t::f - x_t::f>(x.internal) / z)};
}

template <u8 B, i8 I, typename S>
pure auto
operator<=>(t<B, I, S> const& x, t<B, I, S> const& z) noexcept -> std::strong_ordering {
  return (+x) <=> +z;
}
template <u8 B, i8 I, typename S>
pure auto
operator==(t<B, I, S> const& x, t<B, I, S> const& z) noexcept -> bool {
  return (x.internal) == z.internal;
}

// NOLINTEND(cppcoreguidelines-macro-usage)
#pragma clang diagnostic pop

}  // namespace fp

#ifndef NDEBUG
template <u8 B, i8 I, typename S>
requires (B <= kSystemBits)
auto
operator<<(std::ostream& os, fp::t<B, I, S> const& x) -> std::ostream& {
  return os << std::setprecision(2) << std::fixed << std::right << std::setw(8) << static_cast<std::string>(x);  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}
template <ufull_t N, u8 B, i8 I, typename S>
requires (B <= kSystemBits)
auto
operator<<(std::ostream& os, fp::a<N, B, I, S> const& x) -> std::ostream& {
  os << '(' << x[0];
  for (ufull_t i = 1; i < N; ++i) { os << ", " << x[i]; }
  return os << ')';
}
#endif

template <u8 B = pxidx_bits>
requires ((B <= kSystemBits) and (B >= pxidx_bits))
using pxidx_safe_t = fp::t<B, B, unsigned>;
template <u8 B = pxidx_bits>
requires ((B <= kSystemBits) and (B >= pxidx_bits))
using pxidx_exac_t = fp::t<B, pxidx_bits, unsigned>;
template <ufull_t N, u8 B = pxidx_bits>
requires ((B <= kSystemBits) and (B >= pxidx_bits))
using pxidx_safe_a = fp::a<N, B, B, unsigned>;
template <ufull_t N, u8 B = pxidx_bits>
requires ((B <= kSystemBits) and (B >= pxidx_bits))
using pxidx_exac_a = fp::a<N, B, pxidx_bits, unsigned>;

template <u8 B = mm_bits + 1>
requires ((B <= kSystemBits) and (B >= mm_bits + 1))
using mm_safe_t = fp::t<B, B - 1, signed>;
template <u8 B = byte_ceil<mm_bits + 1>>
requires ((B <= kSystemBits) and (B >= byte_ceil<mm_bits + 1>))
using mm_exac_t = fp::t<B, mm_bits, signed>;
template <ufull_t N, u8 B = mm_bits + 1>
requires ((B <= kSystemBits) and (B >= mm_bits + 1))
using mm_safe_a = fp::a<N, B, B - 1, signed>;
template <ufull_t N, u8 B = byte_ceil<mm_bits + 1>>
requires ((B <= kSystemBits) and (B >= byte_ceil<mm_bits + 1>))
using mm_exac_a = fp::a<N, B, mm_bits, signed>;
