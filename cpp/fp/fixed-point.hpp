#pragma once

#include "util/array-inits.hpp"
#include "util/byte-ceil.hpp"
#include "util/fixed-string.hpp"
#include "util/ints.hpp"
#include "util/shift.hpp"
#include "util/ternary.hpp"
#include "util/uninitialized.hpp"
#include "util/units.hpp"

#include <array>

#ifndef NDEBUG
#include <iostream>
#endif

namespace fp {
template <u8 B, u8 I, typename S> class t;
template <std::size_t N, u8 B, u8 I, typename S> class a;
}  // namespace fp

template <typename T> struct is_fixed_point_s {
  static constexpr bool value = false;
};
template <u8 B, u8 I, typename S> struct is_fixed_point_s<fp::t<B, I, S>> {
  static constexpr bool value = true;
};
template <std::size_t N, u8 B, u8 I, typename S> struct is_fixed_point_s<fp::a<N, B, I, S>> {
  static constexpr bool value = true;
};
template <typename T> static constexpr bool is_fixed_point = is_fixed_point_s<std::decay_t<T>>::value;
template <typename T> concept FixedPoint = is_fixed_point<T>;

template <typename T> struct is_fixed_point_array_s {
  static constexpr bool value = false;
};
template <std::size_t N, u8 B, u8 I, typename S> struct is_fixed_point_array_s<fp::a<N, B, I, S>> {
  static constexpr bool value = true;
};
template <typename T> static constexpr bool is_fixed_point_array = is_fixed_point_array_s<std::decay_t<T>>::value;
template <typename T> concept FixedPointArray = is_fixed_point_array<T>;

namespace fp {

// TODO(wrsturgeon): no particular reason `I` can't be negative
template <u8 B, u8 I, typename S> class t {
 public:
  using int_t = cint<byte_ceil(B), S>;
  using self_t = t<B, I, S>;
  static constexpr u8 b = B;
  static constexpr u8 i = I;
  static constexpr bool s = std::is_signed_v<S>;
  static constexpr u8 f = b - i - s;
 private:
  int_t internal;
 public:
  constexpr explicit t(int_t x) : internal{x} {}
  pure static auto zero() -> self_t { return self_t{0}; }
  /* clang-format off */ template <i8 p> pure static auto p2() -> self_t requires ((p < i) and (f + p >= 0)) { return self_t{1 << (f + p)}; } /* clang-format on */
  pure static auto unit() -> self_t { return p2<0>(); }
  pure auto operator+() const noexcept -> int_t { return internal; }
  pure explicit operator int_t() const noexcept { return internal; }
  template <u8 B2, u8 I2, typename S2> pure explicit operator t<B2, I2, S2>() const noexcept;
  pure auto floor() const noexcept -> int_t { return internal >> f; }
  pure auto round() const noexcept -> int_t { return (p2<-1>() + *this).floor(); }
  pure auto squared() const noexcept -> self_t { return self_t((internal * internal) >> f); }
};

template <std::size_t N, u8 B, u8 I, typename S> class a {
 public:
  using int_t = cint<byte_ceil(B), S>;
  using arr_t = std::array<int_t, N>;
  using self_t = a<N, B, I, S>;
  using scalar_t = t<B, I, S>;
  static constexpr std::size_t n = N;
  static constexpr u8 b = B;
  static constexpr u8 i = I;
  static constexpr bool s = std::is_signed_v<S>;
  static constexpr u8 f = b - i - s;
 private:
  arr_t internal;
  template <std::size_t N2, u8 B2, u8 I2, typename S2> friend class a;
 public:
  constexpr explicit a(arr_t const& x) : internal{x} {}
  constexpr explicit a(arr_t&& x) : internal{std::move(x)} {}
  template <typename... T> constexpr explicit a(T&&... x) : internal{static_cast<int_t>(std::forward<T>(x))...} {}
  pure static auto zero() -> self_t { return self_t{zeros<int_t, N>()}; }
  pure auto operator+() const noexcept -> arr_t { return internal; }
  pure auto operator[](std::size_t idx) const -> scalar_t { return scalar_t{internal[idx]}; }
  pure auto begin() const noexcept -> decltype(auto) { return internal.begin(); }
  pure auto end() const noexcept -> decltype(auto) { return internal.end(); }
};

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert(sizeof(t<8, 0, signed>) == sizeof(typename t<8, 0, signed>::int_t));
static_assert(sizeof(a<1, 8, 0, signed>) == sizeof(typename a<1, 8, 0, signed>::int_t));
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

template <u8 B, u8 I, typename S> template <u8 B2, u8 I2, typename S2>
pure t<B, I, S>::operator t<B2, I2, S2>() const noexcept {
  using t2 = t<B2, I2, S2>;
  return t2{static_cast<typename t2::int_t>(lshift<t2::f - f>(static_cast<std::ptrdiff_t>(internal)))};
}

#define DEFINE_BINARY_OP_RETURNING(RETURNTYPE, SYMBOL, ...)                                                                   \
  template <u8 B1, u8 I1, typename S1, u8 B2, u8 I2, typename S2>                                                             \
  pure auto operator SYMBOL(t<B1, I1, S1> const& x, t<B2, I2, S2> const& y)->RETURNTYPE {                                     \
    static constexpr auto F1 = t<B1, I1, S1>::f;                                                                              \
    static constexpr auto F2 = t<B2, I2, S2>::f;                                                                              \
    return RETURNTYPE(__VA_ARGS__);                                                                                           \
  }                                                                                                                           \
  template <u8 B1, u8 I1, typename S1> pure auto operator SYMBOL(t<B1, I1, S1> const& x, std::integral auto y)->RETURNTYPE {  \
    return operator SYMBOL(x, from_int(y));                                                                                   \
  }
#define DEFAULT_RETURN_TYPE t<B1, I1, S1>
#define DEFINE_BINARY_OP(SYMBOL, ...) DEFINE_BINARY_OP_RETURNING(DEFAULT_RETURN_TYPE, SYMBOL, __VA_ARGS__)
DEFINE_BINARY_OP(+, (+x) + rshift<F2 - F1>(+y))
DEFINE_BINARY_OP(-, (+x) - rshift<F2 - F1>(+y))
DEFINE_BINARY_OP(*, ((+x) * (+y)) >> F2);
DEFINE_BINARY_OP(/, ((+x) << F2) / (+y));
DEFINE_BINARY_OP(>>, (+x) >> y.floor())
DEFINE_BINARY_OP_RETURNING(bool, ==, (+x) == rshift<F2 - F1>(+y))  // Note that this is not necessarily commutative!
DEFINE_BINARY_OP_RETURNING(bool, !=, (+x) != rshift<F2 - F1>(+y))

#define DEFINE_UNARY_OP(SYMBOL, ...)                                                                                          \
  template <u8 B, u8 I, typename S> pure auto operator SYMBOL(t<B, I, S> const& x)->t<B, I, S> {                              \
    static constexpr auto F = t<B, I, S>::f;                                                                                  \
    return t<B, I, S>(__VA_ARGS__);                                                                                           \
  }
DEFINE_UNARY_OP(-, -(+x))
DEFINE_UNARY_OP(~, ~(+x))

// NOLINTEND(cppcoreguidelines-macro-usage)
#pragma clang diagnostic pop

}  // namespace fp

template <u8 B, u8 I, typename S> auto
operator<<(std::ostream& os, fp::t<B, I, S> const& x) -> std::ostream& {
  return os << (static_cast<double>(+x) / (1 << fp::t<B, I, S>::f));
}
template <std::size_t N, u8 B, u8 I, typename S> auto
operator<<(std::ostream& os, fp::a<N, B, I, S> const& x) -> std::ostream& {
  os << '(' << x[0];
  for (std::size_t i = 1; i < N; ++i) { os << ", " << x[i]; }
  return os << ')';
}
