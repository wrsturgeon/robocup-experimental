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
#include <iomanip>
#include <iostream>
#endif

namespace fp {
template <u8 B, i8 I, typename S> class t;
template <std::size_t N, u8 B, i8 I, typename S> class a;
}  // namespace fp

template <typename T> struct is_fixed_point_s {
  static constexpr bool value = false;
};
template <u8 B, i8 I, typename S> struct is_fixed_point_s<fp::t<B, I, S>> {
  static constexpr bool value = true;
};
template <std::size_t N, u8 B, i8 I, typename S> struct is_fixed_point_s<fp::a<N, B, I, S>> {
  static constexpr bool value = true;
};
template <typename T> static constexpr bool is_fixed_point = is_fixed_point_s<std::decay_t<T>>::value;
template <typename T> concept FixedPoint = is_fixed_point<T>;

template <typename T> struct is_fixed_point_array_s {
  static constexpr bool value = false;
};
template <std::size_t N, u8 B, i8 I, typename S> struct is_fixed_point_array_s<fp::a<N, B, I, S>> {
  static constexpr bool value = true;
};
template <typename T> static constexpr bool is_fixed_point_array = is_fixed_point_array_s<std::decay_t<T>>::value;
template <typename T> concept FixedPointArray = is_fixed_point_array<T>;

namespace fp {

// TODO(wrsturgeon): no particular reason `I` can't be negative
template <u8 B, i8 I, typename S> class t {
 public:
  static constexpr u8 b = B;
  static constexpr i8 i = I;
  static constexpr bool s = std::is_signed_v<S>;
  static constexpr i8 f = b - i - s;
  using internal_t = cint<byte_ceil(B), S>;
  using floor_t = cint<byte_ceil(i + s), S>;
  using self_t = t<B, I, S>;
 private:
  internal_t internal;
 public:
  constexpr explicit t(internal_t x) : internal{x} {}
  pure static auto zero() -> self_t { return self_t{0}; }
  template <i8 p> requires (p < i)  // and (f + p >= 0))
  pure static auto p2() -> self_t {
    return self_t(lshift<f + p, std::size_t>(1));
  }
  pure static auto unit() -> self_t { return p2<0>(); }
  pure auto operator+() const noexcept -> internal_t { return internal; }
  pure explicit operator internal_t() const noexcept { return internal; }
  template <u8 B2, i8 I2, typename S2> pure explicit operator t<B2, I2, S2>() const noexcept;
  pure auto floor() const noexcept -> floor_t { return static_cast<floor_t>(rshift<f, ptrdiff_t>(internal)); }
  pure auto round() const noexcept -> floor_t { return (p2<-1>() + *this).floor(); }
  pure auto squared() const noexcept -> t<B, (I << 1), S> { return t<B, (I << 1), S>((internal * internal) >> f); }
};

template <std::size_t N, u8 B, i8 I, typename S> class a {
 public:
  static constexpr std::size_t n = N;
  static constexpr u8 b = B;
  static constexpr i8 i = I;
  static constexpr bool s = std::is_signed_v<S>;
  static constexpr i8 f = b - i - s;
  using scalar_t = t<B, I, S>;
  using internal_t = typename scalar_t::internal_t;
  using floor_t = cint<byte_ceil(i + s), S>;
  using arr_t = std::array<internal_t, N>;
  using self_t = a<N, B, I, S>;
 private:
  arr_t internal;
  template <std::size_t N2, u8 B2, i8 I2, typename S2> friend class a;
 public:
  // constexpr explicit a(arr_t const& x) : internal{x} {}
  // constexpr explicit a(arr_t&& x) : internal{std::move(x)} {}
  // template <typename... T> constexpr explicit a(T&&... x) : internal{static_cast<internal_t>(std::forward<T>(x))...} {}
  template <FixedPoint... T> constexpr explicit a(T&&... x) : internal{+static_cast<scalar_t>(std::forward<T>(x))...} {}
  pure static auto zero() -> self_t { return self_t{zeros<internal_t, N>()}; }
  pure auto operator+() const noexcept -> arr_t { return internal; }
  pure auto operator[](std::size_t idx) const -> scalar_t { return scalar_t{internal[idx]}; }
  pure auto begin() const noexcept -> decltype(auto) { return internal.begin(); }
  pure auto end() const noexcept -> decltype(auto) { return internal.end(); }
  pure auto floor() const noexcept -> std::array<floor_t, N>;
  pure auto round() const noexcept -> std::array<floor_t, N>;
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

template <u8 B, i8 I, typename S> template <u8 B2, i8 I2, typename S2>
pure t<B, I, S>::operator t<B2, I2, S2>() const noexcept {
  using t2 = t<B2, I2, S2>;
  return t2{static_cast<typename t2::internal_t>(lshift<t2::f - f>(static_cast<std::ptrdiff_t>(internal)))};
}

#define DEFINE_BINARY_OP(SYMBOL, ...)                                                                                         \
  template <u8 B1, i8 I1, typename S1, u8 B2, i8 I2, typename S2>                                                             \
  pure auto operator SYMBOL(t<B1, I1, S1> const& x, t<B2, I2, S2> const& y)->decltype(auto) {                                 \
    constexpr auto F1 = t<B1, I1, S1>::f;                                                                                     \
    constexpr auto F2 = t<B2, I2, S2>::f;                                                                                     \
    return RETURNTYPE(__VA_ARGS__);                                                                                           \
  }                                                                                                                           \
  template <u8 B1, i8 I1, typename S1>                                                                                        \
  pure auto operator SYMBOL(t<B1, I1, S1> const& x, std::integral auto y)->decltype(auto) {                                   \
    return operator SYMBOL(x, from_int(y));                                                                                   \
  }
template <typename A, typename B>
using either_signed = std::conditional_t<std::is_signed_v<A> or std::is_signed_v<B>, signed, unsigned>;
#define RETURNTYPE t<B1, I1, S1>
// #define RETURNTYPE t<B1, std::max(I1, I2) + 1, either_signed<S1, S2>>
DEFINE_BINARY_OP(+, (+x) + (+static_cast<t<B1, I1, S1>>(y)))
// #undef RETURNTYPE
// #define RETURNTYPE t<B1, std::max(I1, I2) + 1, signed>
DEFINE_BINARY_OP(-, (+x) - (+static_cast<t<B1, I1, S1>>(y)))
// #undef RETURNTYPE
// #define RETURNTYPE t<B1, I1 + I2, either_signed<S1, S2>>
DEFINE_BINARY_OP(*, static_cast<typename RETURNTYPE::internal_t>(rshift<F2, std::ptrdiff_t>(static_cast<std::ptrdiff_t>(+x) * static_cast<std::ptrdiff_t>(+y))));
// #undef RETURNTYPE
// #define RETURNTYPE t<B1, I1 + F2, either_signed<S1, S2>>
DEFINE_BINARY_OP(/, static_cast<typename RETURNTYPE::internal_t>(lshift<F2, std::ptrdiff_t>(+x) / (+y)));
// #undef RETURNTYPE
// #define RETURNTYPE t<B1, I1, S1>
DEFINE_BINARY_OP(>>, (+x) >> y.round())
DEFINE_BINARY_OP(<<, (+x) << y.round())
#undef RETURNTYPE
#define RETURNTYPE bool
DEFINE_BINARY_OP(==, (+x) == rshift<F2 - F1>(+y))  // Note that this is not necessarily commutative!
DEFINE_BINARY_OP(!=, (+x) != rshift<F2 - F1>(+y))
#undef RETURNTYPE
#define RETURNTYPE std::strong_ordering
DEFINE_BINARY_OP(<=>, (+x) <=> rshift<F2 - F1>(+y))
#undef RETURNTYPE

#define RETURNTYPE t<B, I, S>
#define DEFINE_UNARY_OP(SYMBOL, ...)                                                                                          \
  template <u8 B, i8 I, typename S> pure auto operator SYMBOL(t<B, I, S> const& x)->RETURNTYPE {                              \
    constexpr auto F = t<B, I, S>::f;                                                                                         \
    return RETURNTYPE(__VA_ARGS__);                                                                                           \
  }
DEFINE_UNARY_OP(-, -(+x))
DEFINE_UNARY_OP(~, ~(+x))

// NOLINTEND(cppcoreguidelines-macro-usage)
#pragma clang diagnostic pop

static constexpr u8 kPrintWidth = 8;

}  // namespace fp

#ifndef NDEBUG
template <u8 B, i8 I, typename S> auto
operator<<(std::ostream& os, fp::t<B, I, S> const& x) -> std::ostream& {
  return os << std::setprecision(2) << std::fixed << std::right << std::setw(fp::kPrintWidth)
            << (static_cast<double>(+x) / (1 << fp::t<B, I, S>::f));
}
template <std::size_t N, u8 B, i8 I, typename S> auto
operator<<(std::ostream& os, fp::a<N, B, I, S> const& x) -> std::ostream& {
  os << '(' << x[0];
  for (std::size_t i = 1; i < N; ++i) { os << ", " << x[i]; }
  return os << ')';
}
#endif
