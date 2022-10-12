#pragma once

#include "util/custom-int.hpp"
#include "util/ternary.hpp"
#include "util/uninitialized.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cmath>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>

#ifndef NDEBUG
#include <iostream>
#include <string>
#endif

namespace fp {

// NOLINTBEGIN(clang-diagnostic-implicit-int-conversion)
// NOLINTBEGIN(clang-diagnostic-shift-sign-overflow)
// NOLINTBEGIN(clang-diagnostic-shorten-64-to-32)
// NOLINTBEGIN(clang-diagnostic-sign-conversion)
// NOLINTBEGIN(clang-diagnostic-unused-function)

// TODO(wrsturgeon): use expression templates!
// TODO(wrsturgeon): And set this up as a custom type in Eigen:
// http://eigen.tuxfamily.org/dox/TopicCustomizing_CustomScalar.html

// Forward declarations
template <std::uint8_t b, std::uint8_t f, typename u>
class t;
template <std::size_t N, typename T>
class a;

#define OTHERS template <std::uint8_t b2, std::uint8_t f2, typename u2>
#define other_t t<b2, f2, u2>

template <std::uint8_t b, std::uint8_t f = (b >> 1), typename u = signed>
class t {
  static_assert((b & (b - 1)) == 0, "b must be a power of 2");
  // static_assert(f <= b - std::is_signed_v<u>, "Fractional bits > total - ?sign bit"); // this is actually okay
 private:
  using self_t = t<b, f, u>;
  using signed_t = t<b, f, signed>;
  using T = cint<b, u>;
  T internal;
  explicit constexpr t(T x) noexcept : internal{x} {}
 public:
  static constexpr auto fbits = f;
  static constexpr auto sbits = static_cast<std::uint8_t>(std::is_signed_v<u>);
  static constexpr auto ibits = b - f - sbits;
  template <std::size_t N>
  using array = a<N, self_t>;
  template <std::uint8_t b2, std::uint8_t f2, typename u2>
  friend class t;
  pure static auto zero() -> self_t { return self_t{0}; }
  pure static auto p2(std::int8_t p) -> self_t;
  pure static auto unit() -> self_t { return p2(0); }
  pure explicit operator bool() const { return static_cast<bool>(internal); }
  OTHERS pure explicit operator t<b2, f2, u2>() const;
  OTHERS pure auto operator==(t<b2, f2, u2> const& x) const -> bool;
  OTHERS pure auto operator!=(t<b2, f2, u2> const& x) const -> bool;
  OTHERS pure auto operator<=>(t<b2, f2, u2> const& x) const -> std::strong_ordering;
  pure auto operator-() const -> signed_t;
  pure auto operator~() const -> self_t { return self_t{~internal}; }
  pure auto operator|(T x) const -> self_t { return self_t{static_cast<T>(internal | x)}; }
  pure auto operator<<(std::uint8_t const& x) const -> self_t;
  pure auto operator>>(std::uint8_t const& x) const -> self_t;
  INLINE auto operator<<=(std::uint8_t const& x) -> self_t& { return *this = operator<<(x); }
  INLINE auto operator>>=(std::uint8_t const& x) -> self_t& { return *this = operator>>(x); }
  OTHERS pure auto operator+(other_t const& x) const -> self_t;
  OTHERS pure auto operator-(other_t const& x) const -> self_t;
  OTHERS pure auto operator*(other_t const& x) const -> self_t;
  OTHERS pure auto operator/(other_t const& x) const -> self_t;
  OTHERS INLINE auto operator+=(other_t const& x) -> self_t& { return *this = operator+(x); }
  OTHERS INLINE auto operator-=(other_t const& x) -> self_t& { return *this = operator-(x); }
  OTHERS INLINE auto operator*=(other_t const& x) -> self_t& { return *this = operator*(x); }
  OTHERS INLINE auto operator/=(other_t const& x) -> self_t& { return *this = operator/(x); }
  pure auto sqrt() const -> self_t;
  pure auto abs() const -> self_t { return self_t{std::abs(internal)}; }
#ifndef NDEBUG
  strpure explicit operator std::string() const { return std::to_string(ldexp(internal, -f)); }
  strpure static auto typestr() -> std::string;
  strpure auto expose() const -> std::string;
#endif  // NDEBUG
};

#undef other_t
#undef OTHERS

template <std::uint8_t b1, std::uint8_t f1, typename u1>
template <std::uint8_t b2, std::uint8_t f2, typename u2>
constexpr t<b1, f1, u1>::operator t<b2, f2, u2>() const {
  using rtn_t = t<b2, f2, u2>;
#ifndef NDEBUG
  if constexpr (std::is_signed_v<u1> && std::is_unsigned_v<u2>) {
    if (internal < 0) {
      throw std::out_of_range{"Can't convert (negative) " + expose() + " to (unsigned) " + rtn_t::typestr()};
    }
  }
  constexpr std::int8_t df = f2 - f1;
  constexpr std::int8_t ibit_loss = self_t::ibits - rtn_t::ibits;
  if constexpr (ibit_loss > 0) {
    if (((internal < 0) ? ~internal : internal) >> (b1 - ibit_loss)) {
      throw std::out_of_range{"Can't convert " + expose() + " to " + rtn_t::typestr() + ": overflow"};
    }
  }
#endif  // NDEBUG
  if constexpr (df < 0) {
    return rtn_t{static_cast<typename rtn_t::T>(internal >> -df)};
  } else {
    return rtn_t{static_cast<typename rtn_t::T>(internal << df)};
  }
}

template <std::uint8_t b, std::uint8_t f, typename u>
constexpr auto t<b, f, u>::p2(std::int8_t p) -> self_t {
#ifndef NDEBUG
  if (b - f - std::is_signed_v<u> <= p) {
    throw std::out_of_range{"Can't store 2^p: overflow"};
  }
  if (p < -f) {
    throw std::out_of_range{"Initializing an t<...> with a (literally) vanishingly small power of 2"};
  }
#endif  // NDEBUG
  auto dp = p + f;
  // NOLINTBEGIN(clang-diagnostic-shift-count-*)
  if (dp < 0) {
    return self_t{static_cast<T>(1 >> -dp)};
  }
  return self_t{static_cast<T>(1 << dp)};
  // NOLINTEND(clang-diagnostic-shift-count-*)
}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ADDCOMP(COMPARATOR, return_t)                                                                                         \
  template <std::uint8_t b1, std::uint8_t f1, typename u1>                                                                    \
  template <std::uint8_t b2, std::uint8_t f2, typename u2>                                                                    \
  constexpr auto t<b1, f1, u1>::operator COMPARATOR(t<b2, f2, u2> const& x) const->return_t {                                 \
    using u3 = std::conditional_t<std::is_signed_v<u1> || std::is_signed_v<u2>, signed, unsigned>;                            \
    using other_t = t<b2, f2, u2>;                                                                                            \
    constexpr std::uint8_t s3 = std::is_signed_v<u3>;                                                                         \
    constexpr std::uint8_t i3 = other_t::ibits > ibits ? other_t::ibits : ibits;                                              \
    using t3 = t<std::bit_ceil<std::uint8_t>(s3 + i3 + f1), f1, u3>;                                                          \
    return (operator t3()).internal COMPARATOR static_cast<t3>(x).internal;                                                   \
  }
ADDCOMP(==, bool)
ADDCOMP(!=, bool)
ADDCOMP(<=>, std::strong_ordering)
#undef ADDCOMP

template <std::uint8_t b, std::uint8_t f, typename u>
constexpr auto t<b, f, u>::operator-() const -> signed_t {
#ifndef NDEBUG
  if constexpr (std::is_unsigned_v<u>) {
    if (static_cast<std::make_signed_t<T>>(-internal) > 0) {
      throw std::out_of_range{"Can't negate " + expose() + ": overflow"};
    }
  } else {
    if (internal == std::numeric_limits<T>::min()) {
      throw std::out_of_range{"Can't negate " + expose() + ": overflow"};
    }
  }
#endif  // NDEBUG
  return signed_t{static_cast<std::make_signed_t<T>>(-internal)};
}

template <std::uint8_t b1, std::uint8_t f1, typename u1>
template <std::uint8_t b2, std::uint8_t f2, typename u2>
constexpr auto t<b1, f1, u1>::operator+(t<b2, f2, u2> const& x) const -> self_t {
  auto const y = static_cast<self_t>(x);
#ifndef NDEBUG
  using uT = std::make_unsigned_t<T>;
  // inrange: e.g. for 8b signed ints, it's [-128, 127] + 128 -> [0, 255]
  uT const inrange = ifc<
        std::is_unsigned_v<u1>>(static_cast<uT>(internal), static_cast<uT>(internal ^ static_cast<T>(1U << (b1 - 1))));
  if (x.internal < 0) {
    return operator-(-x);
  }  // should be optimized away if y is unsigned
  if (static_cast<uT>(y.internal) > static_cast<uT>(~inrange)) {
    throw std::out_of_range{"Evaluating (" + expose() + " + " + y.expose() + ") into an " + typestr() + " will overflow"};
  }
#endif  // NDEBUG
  return self_t{static_cast<T>(internal + y.internal)};
}

template <std::uint8_t b1, std::uint8_t f1, typename u1>
template <std::uint8_t b2, std::uint8_t f2, typename u2>
constexpr auto t<b1, f1, u1>::operator-(t<b2, f2, u2> const& x) const -> self_t {
  auto const y = static_cast<self_t>(x);
#ifndef NDEBUG
  using uT = std::make_unsigned_t<T>;
  // inrange: e.g. for 8b signed ints, it's [-128, 127] + 128 -> [0, 255]
  auto const inrange = ifc<
        std::is_unsigned_v<u1>>(static_cast<uT>(internal), static_cast<uT>(internal ^ static_cast<T>(1U << (b1 - 1))));
  if (x.internal < 0) {
    return operator+(-x);
  }  // should be optimized away if y is unsigned
  if (static_cast<uT>(y.internal) > inrange) {
    throw std::out_of_range{"Evaluating (" + expose() + " - " + y.expose() + ") into an " + typestr() + " will overflow"};
  }
#endif  // NDEBUG
  return self_t{static_cast<T>(internal - y.internal)};
}

template <std::uint8_t b1, std::uint8_t f1, typename u1>
template <std::uint8_t b2, std::uint8_t f2, typename u2>
constexpr auto t<b1, f1, u1>::operator*(t<b2, f2, u2> const& x) const -> self_t {
  // TODO(wrsturgeon)
  // assert(
  //       static_cast<T>((static_cast<ext_t>(internal) * static_cast<ext_t>(x.internal)) >> f) ==
  //       static_cast<ext_t>((static_cast<ext_t>(internal) * static_cast<ext_t>(x.internal)) >> f));
  // auto y = static_cast<self_t>(x);
  // if constexpr (cint_exists<std::bit_ceil<std::uint8_t>(b1 + f1)>) {
  //   using ext_t = cint<std::bit_ceil<std::uint8_t>(b1 + f1), u1>;
  //   return self_t{static_cast<T>((static_cast<ext_t>(internal) * static_cast<ext_t>(y.internal)) >> f1)};
  // } else {
  //   assert(false);  // TODO(wrsturgeon)
  // }
  return static_cast<self_t>(x);
}

template <std::uint8_t b1, std::uint8_t f1, typename u1>
template <std::uint8_t b2, std::uint8_t f2, typename u2>
constexpr auto t<b1, f1, u1>::operator/(t<b2, f2, u2> const& x) const -> self_t {
#ifndef NDEBUG
  if (!x) {
    throw std::out_of_range{"Division by zero"};
  }
  // TODO(wrsturgeon)
  // assert(
  //       static_cast<T>((static_cast<full_t>(internal) << f) / x.internal) ==
  //       static_cast<full_t>((static_cast<full_t>(internal) << f) / x.internal));
#endif
  // return self_t{static_cast<T>((static_cast<full_t>(internal) << f) / x.internal)};
  return static_cast<self_t>(x);
}

template <std::uint8_t b, std::uint8_t f, typename u>
constexpr auto t<b, f, u>::operator<<(std::uint8_t const& x) const -> self_t {
  return self_t{static_cast<T>(internal << x)};
}

template <std::uint8_t b, std::uint8_t f, typename u>
constexpr auto t<b, f, u>::operator>>(std::uint8_t const& x) const -> self_t {
  return self_t{static_cast<T>(internal >> x)};
}

template <std::uint8_t b, std::uint8_t f, typename u>
constexpr auto t<b, f, u>::sqrt() const -> self_t {
  // Babylonian method
  if (!*this) {
    return self_t::zero();
  }
  self_t x = *this;
  constexpr std::uint8_t N = std::bit_width<std::uint8_t>(b);
#pragma unroll
  for (std::uint8_t i = 0; i < N; ++i) {
    x = ((x + ((*this) / x)) >> 1);
  }
  return x;
}

#ifndef NDEBUG

template <std::uint8_t b, std::uint8_t f, typename u>
strpure auto operator+(std::string const& s, fp::t<b, f, u> const& x) -> std::string {
  return s + static_cast<std::string>(x);
}

template <std::uint8_t b, std::uint8_t f, typename u>
strpure auto fp::t<b, f, u>::typestr() -> std::string {
  return "fp::t<" + std::to_string(b) + ", " + std::to_string(f) + ", " + (std::is_signed_v<u> ? "signed" : "unsigned") + '>';
}

template <std::uint8_t b, std::uint8_t f, typename u>
strpure auto operator<<(std::ostream& os, t<b, f, u> const& p) -> std::ostream& {
  return os << static_cast<std::string>(p);
}

template <std::uint8_t b, std::uint8_t f, typename u>
strpure auto t<b, f, u>::expose() const -> std::string {
  return operator std::string() + " [" + std::to_string(b) + 'b' + std::to_string(f) + "f:" + std::to_string(internal) + "]";
}

#endif  // NDEBUG

template <typename func, std::size_t N, std::uint8_t b, std::uint8_t f, typename u>
pure auto foreach (a<N, t<b, f, u>> const& x) -> a<N, t<b, f, u>> {
  auto r = uninitialized<a<N, t<b, f, u>>>();
#pragma unroll
  for (std::size_t i = 0; i < N; ++i) {
    r[i] = func(x[i]);
  }
  return r;
}

template <typename func, std::size_t N, std::uint8_t b, std::uint8_t f, typename u>
pure auto foreach (a<N, t<b, f, u>> const& x, a<N, t<b, f, u>> const& y) -> a<N, t<b, f, u>> {
  auto r = uninitialized<a<N, t<b, f, u>>>();
#pragma unroll
  for (std::size_t i = 0; i < N; ++i) {
    r[i] = func(x[i], y[i]);
  }
  return r;
}

template <typename func, std::size_t N, std::uint8_t b, std::uint8_t f, typename u>
pure auto foreach (a<N, t<b, f, u>> const& x, t<b, f, u> const& y) -> a<N, t<b, f, u>> {
  auto r = uninitialized<a<N, t<b, f, u>>>();
#pragma unroll
  for (std::size_t i = 0; i < N; ++i) {
    r[i] = func(x[i], y);
  }
  return r;
}

template <typename func, std::size_t N, std::uint8_t b, std::uint8_t f, typename u>
pure auto foreach (t<b, f, u> const& x, a<N, t<b, f, u>> const& y) -> a<N, t<b, f, u>> {
  auto r = uninitialized<a<N, t<b, f, u>>>();
#pragma unroll
  for (std::size_t i = 0; i < N; ++i) {
    r[i] = func(x, y[i]);
  }
  return r;
}

// clang-format off
// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define  UNARY_OP(...) pure auto operator __VA_ARGS__(               ) const -> self_t { return foreach<fp_t::operator __VA_ARGS__>(*this   ); }
#define BINARY_OP(...) pure auto operator __VA_ARGS__(self_t const& x) const -> self_t { return foreach<fp_t::operator __VA_ARGS__>(*this, x); } \
                       pure auto operator __VA_ARGS__(fp_t   const& x) const -> self_t { return foreach<fp_t::operator __VA_ARGS__>(*this, x); }
#define OTHER_TEMPLATE template <std::uint8_t b2, std::uint8_t f2, typename u2>
// NOLINTEND(cppcoreguidelines-macro-usage)
// clang-format on

template <std::size_t N, std::uint8_t b, std::uint8_t f, typename u>
class a<N, t<b, f, u>> {
 private:
  using fp_t = t<b, f, u>;
  using arr_t = std::array<fp_t, N>;
  using self_t = a<N, fp_t>;
  arr_t internal;
 public:
  constexpr a(std::initializer_list<fp_t>&& li) NOX;
  OTHER_TEMPLATE pure explicit operator a<N, t<b2, f2, u2>>() const;
  pure auto begin() noexcept -> fp_t* { return internal.begin(); }
  pure auto end() noexcept -> fp_t* { return internal.end(); }
  pure auto begin() const noexcept -> fp_t const* { return internal.begin(); }
  pure auto end() const noexcept -> fp_t const* { return internal.end(); }
  pure auto operator[](std::size_t i) -> fp_t&;
  pure static auto zero() -> self_t;
  template <std::size_t i>
  pure static auto unit() -> self_t;
  pure explicit operator bool() const;
  pure auto r2() const -> fp_t;
  pure auto r() const -> fp_t { return r2().sqrt(); }
  pure auto sqrt() const -> self_t;
  UNARY_OP(-)
  UNARY_OP(~)  //
  BINARY_OP(+) BINARY_OP(-) BINARY_OP(*) BINARY_OP(/) BINARY_OP(<<) BINARY_OP(>>)
};

#undef OTHER_TEMPLATE
#undef BINARY_OP
#undef UNARY_OP

template <std::size_t N, std::uint8_t b, std::uint8_t f, typename u>
constexpr a<N, t<b, f, u>>::a(std::initializer_list<fp_t>&& li) NOX : internal{uninitialized<arr_t>()} {
#ifndef NDEBUG
  if (li.size() != N) {
    throw std::invalid_argument("Initializer list size does not match array size");
  }
#endif
  std::copy(li.begin(), li.end(), this->begin());
}

template <std::size_t N, std::uint8_t b, std::uint8_t f, typename u>
constexpr auto a<N, t<b, f, u>>::operator[](std::size_t i) -> fp_t& {
#ifndef NDEBUG
  if (i < N) {
    throw std::out_of_range{"Indexing into " + std::to_string(i) + "th element of " + std::to_string(N) + "-element fp::a"};
  }
#endif
  return internal[i];
}

template <std::size_t N, std::uint8_t b, std::uint8_t f, typename u>
constexpr auto a<N, t<b, f, u>>::zero() -> self_t {
  auto r = uninitialized<self_t>();
  r.internal.fill(fp_t::zero());
  return r;
}

template <std::size_t N, std::uint8_t b, std::uint8_t f, typename u>
template <std::size_t i>
constexpr auto a<N, t<b, f, u>>::unit() -> self_t {
  auto r = self_t::zero();
  r[i] = fp_t::unit();
  return r;
}

template <std::size_t N, std::uint8_t b, std::uint8_t f, typename u>
template <std::uint8_t b2, std::uint8_t f2, typename u2>
constexpr a<N, t<b, f, u>>::operator a<N, t<b2, f2, u2>>() const {
  auto r = uninitialized<a<N, t<b2, f2, u2>>>();
#pragma unroll
  for (std::size_t i = 0; i < N; ++i) {
    r[i] = static_cast<t<b2, f2, u2>>((*this)[i]);
  }
  return r;
}

template <std::size_t N, std::uint8_t b, std::uint8_t f, typename u>
constexpr a<N, t<b, f, u>>::operator bool() const {
  return std::any_of(this->begin(), this->end(), [](auto const& x) { return static_cast<bool>(x); });
}

template <std::size_t N, std::uint8_t b, std::uint8_t f, typename u>
constexpr auto a<N, t<b, f, u>>::r2() const -> fp_t {
  return std::accumulate(this->begin(), this->end(), fp_t::zero(), [](auto const& x, auto const& y) { return x + y * y; });
}

template <std::size_t N, std::uint8_t b, std::uint8_t f, typename u>
constexpr auto a<N, t<b, f, u>>::sqrt() const -> self_t {
  auto r = uninitialized<self_t>();
#pragma unroll
  for (std::size_t i = 0; i < N; ++i) {
    r[i] = (*this)[i].sqrt();
  }
  return r;
}

// NOLINTEND(clang-diagnostic-unused-function)
// NOLINTEND(clang-diagnostic-sign-conversion)
// NOLINTEND(clang-diagnostic-shorten-64-to-32)
// NOLINTEND(clang-diagnostic-shift-sign-overflow)
// NOLINTEND(clang-diagnostic-implicit-int-conversion)

}  // namespace fp
