#pragma once

#include "util/array-inits.hpp"
#include "util/byte-ceil.hpp"
#include "util/fixed-string.hpp"
#include "util/ints.hpp"
#include "util/ternary.hpp"
#include "util/uninitialized.hpp"
#include "util/units.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <limits>
#include <numeric>  // std::accumulate
#include <tuple>
#include <type_traits>

template <typename T>
concept FixedPoint = requires {
                       { T::b } -> std::same_as<u8 const&>;
                       { T::i } -> std::same_as<u8 const&>;
                       { T::f } -> std::same_as<u8 const&>;
                       { T::s } -> std::same_as<bool const&>;
                       { T::decidable } -> std::same_as<bool const&>;
                     };

template <typename T> concept FixedPointArray = FixedPoint<T> and
      requires (T x) {
        { T::n } -> std::same_as<std::size_t const&>;
        x.begin();
        x.end();
      };

pure static auto
all(auto x) -> bool {
  return std::all_of(x.begin(), x.end(), [](auto y) { return y; });
}

pure static auto
none(auto x) -> bool {
  return std::all_of(x.begin(), x.end(), [](auto y) { return !y; });
}

namespace fp {

template <u8 B, u8 I = 0, typename S = signed, bool D = false, cint<byte_ceil(B), S> V = 0>
requires (D or !V)  // undecidable ==> V = 0
class t;

template <
      std::size_t N,
      u8 B,
      u8 I = 0,
      typename S = signed,
      std::array<bool, N> D = zeros<bool, N>(),
      std::array<cint<byte_ceil(B), S>, N> V = zeros<cint<byte_ceil(B), S>, N>()>
requires (all(D) or none(V))
class a;

template <FixedString Name, FixedPoint T1, typename... Tn> struct op;

template <typename T> struct get_b_s {};
template <typename T> constexpr inline static u8 get_b = get_b_s<T>::value;
template <u8 B, u8 I, typename S, bool D, cint<byte_ceil(B), S> V> struct get_b_s<t<B, I, S, D, V>> {
  static constexpr u8 value = B;
};
template <std::size_t N, u8 B, u8 I, typename S, std::array<bool, N> D, std::array<cint<byte_ceil(B), S>, N> V>
struct get_b_s<a<N, B, I, S, D, V>> {
  static constexpr u8 value = B;
};
template <FixedString Name, typename... T> struct get_b_s<op<Name, T...>> {
  static constexpr u8 value = get_b<std::tuple_element_t<0, std::tuple<T...>>>;
};

template <typename T> struct get_i_s {};
template <typename T> constexpr inline static u8 get_i = get_i_s<T>::value;
template <u8 B, u8 I, typename S, bool D, cint<byte_ceil(B), S> V> struct get_i_s<t<B, I, S, D, V>> {
  static constexpr u8 value = I;
};
template <std::size_t N, u8 B, u8 I, typename S, std::array<bool, N> D, std::array<cint<byte_ceil(B), S>, N> V>
struct get_i_s<a<N, B, I, S, D, V>> {
  static constexpr u8 value = I;
};
template <FixedString Name, typename... T> struct get_i_s<op<Name, T...>> {
  static constexpr u8 value = get_i<std::tuple_element_t<0, std::tuple<T...>>>;
};

template <typename T> struct get_s_s {};
template <typename T> constexpr inline static bool get_s = get_s_s<T>::value;
template <u8 B, u8 I, typename S, bool D, cint<byte_ceil(B), S> V> struct get_s_s<t<B, I, S, D, V>> {
  static constexpr bool value = std::is_signed_v<S>;
};
template <std::size_t N, u8 B, u8 I, typename S, std::array<bool, N> D, std::array<cint<byte_ceil(B), S>, N> V>
struct get_s_s<a<N, B, I, S, D, V>> {
  static constexpr bool value = std::is_signed_v<S>;
};
template <FixedString Name, typename... T> struct get_s_s<op<Name, T...>> {
  static constexpr u8 value = get_s<std::tuple_element_t<0, std::tuple<T...>>>;
};

template <typename T> struct get_f_s {};
template <typename T> constexpr inline static u8 get_f = get_f_s<T>::value;
template <u8 B, u8 I, typename S, bool D, cint<byte_ceil(B), S> V> struct get_f_s<t<B, I, S, D, V>> {
  static constexpr u8 value = B - I - get_s<t<B, I, S, D, V>>;
};
template <std::size_t N, u8 B, u8 I, typename S, std::array<bool, N> D, std::array<cint<byte_ceil(B), S>, N> V>
struct get_f_s<a<N, B, I, S, D, V>> {
  static constexpr u8 value = B - I - get_s<a<N, B, I, S, D, V>>;
};
template <FixedString Name, typename... T> struct get_f_s<op<Name, T...>> {
  static constexpr u8 value = get_f<std::tuple_element_t<0, std::tuple<T...>>>;
};

template <u8 i, u8 f, i8 p> requires ((p < i) and (f + p >= 0))  // Vanishingly small (f + p < 0) rounds to zero
pure static auto _p2() -> cint<byte_ceil(i + f), unsigned> {
  return cint<byte_ceil(i + f), unsigned>{std::size_t{1} << (f + p)};
}

// TODO(wrsturgeon): no particular reason `I` can't be negative
template <u8 B, u8 I, typename S, bool D, cint<byte_ceil(B), S> V> requires (D or !V)
class t {
 public:
  using int_t = cint<byte_ceil(B), S>;
  using self_t = t<B, I, S, D, V>;
  using run_t = t<B, I, S, false, 0>;
  using rtn_t = self_t;
  template <int_t v> using compile_t = t<B, I, S, true, v>;
  static constexpr u8 b = get_b<self_t>;
  static constexpr u8 i = get_i<self_t>;
  static constexpr u8 f = get_f<self_t>;
  static constexpr bool s = get_s<self_t>;
  static constexpr bool decidable = D;
 private:
  // std::conditional_t<decidable, bullshit, int_t> internal;
  int_t internal;
 public:  // clang-format off
  constexpr t(int_t x) : internal{x} {} // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr t() noexcept requires decidable : internal{V} {} // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
  pure static auto value() noexcept -> run_t requires decidable { return run_t{V}; }
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  pure operator run_t() const noexcept requires decidable { return value(); }
  pure static auto zero() -> t<B, I, S, true, 0> { return {}; }
  template <i8 p> pure static auto p2() -> t<B, I, S, true, _p2<i, f, p>()> { return {}; }
  pure static auto unit() -> decltype(auto) requires (I > 0) { return t<B, I, S, true, _p2<i, f, 0>()>{}; }  // clang-format on
  pure auto operator+() const noexcept -> int_t { return ifc<decidable>(V, internal); }
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  pure operator bool() const noexcept { return operator+(); }
  template <FixedPoint T> pure auto operator=(T&& x) noexcept -> self_t& requires (
        ((T::decidable and (V == T::V)) or !decidable) and (B == T::b) and (I == T::i) and std::is_same_v<S, typename T::s>);
};

template <u8 B, u8 I, typename S, bool D, cint<byte_ceil(B), S> V>
requires (D or !V)
         template <FixedPoint T> pure auto t<B, I, S, D, V>::operator=(T&& x) noexcept -> self_t& requires (
               ((T::decidable and (V == T::V)) or !decidable) and (B == T::b) and (I == T::i) and
               std::is_same_v<S, typename T::s>)  //
{
  internal = +x;
  return *this;
}

         template <std::size_t N, u8 B, u8 I, typename S, std::array<bool, N> D, std::array<cint<byte_ceil(B), S>, N> V>
         requires (all(D) or none(V))
class a {
 public:
  using int_t = cint<byte_ceil(B), S>;
  using arr_t = std::array<int_t, N>;
  using self_t = a<N, B, I, S, D, V>;
  using run_t = a<N, B, I, S, zeros<bool, N>(), zeros<cint<byte_ceil(B), S>, N>()>;
  using scalar_run_t = t<B, I, S, false, 0>;
  using rtn_t = self_t;
  template <arr_t v> using compile_t = a<N, B, I, S, ones<bool, N>(), v>;
  template <int_t v> using scalar_compile_t = t<B, I, S, true, v>;
  static constexpr std::size_t n = N;
  static constexpr u8 b = B;
  static constexpr u8 i = I;
  static constexpr bool s = std::is_signed_v<S>;
  static constexpr bool decidable = all(D);
  static constexpr u8 f = b - i - s;
 private:
  // std::conditional_t<decidable, bullshit, arr_t> internal;
  arr_t internal;
  template <std::size_t N2, u8 B2, u8 I2, typename S2, std::array<bool, N2> D2, std::array<cint<byte_ceil(B2), S2>, N2> V2>
  requires (all(D2) or none(V2))
  friend class a;
 public:  // clang-format off
  constexpr a(arr_t const& x) : internal{x} {} // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr a(arr_t&& x) : internal{std::move(x)} {} // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
  // using std::forward:
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  template <FixedPoint... T> constexpr a(T&&... x) : internal{+std::forward<T>(x)...} {}
  // template <FixedPoint T> constexpr a(std::initializer_list<T>&& x) : internal{arrayify<T, int_t, N>(std::move(x))} {}
  // template <FixedPoint T> constexpr a(std::initializer_list<T> CONST_IF_RELEASE& x) : internal{arrayify<T, int_t, N>(x)} {}
  // template <FixedPoint T> constexpr a(std::initializer_list<T>&& x) : internal{arrayify<T, int_t, N>(std::move(x))} {}
  constexpr a() noexcept requires decidable : internal{V} {}
  pure static auto value() noexcept -> run_t requires decidable { return run_t{V}; }
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  pure operator run_t() const noexcept requires decidable { return value(); }
  pure static auto zero() -> a<N, B, I, S, ones<bool, N>(), zeros<int_t, N>()> { return {}; }
  template <i8 p> pure static auto p2() -> t<B, I, S, true, _p2<i, f, p>()> { return {}; }
  pure static auto unit() -> decltype(auto) requires (I > 0) { return t<B, I, S, true, _p2<i, f, 0>()>{}; }  // clang-format on
  pure auto operator+() const noexcept -> arr_t { return ifc<decidable>(V, internal); }
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  pure operator bool() const noexcept { return all(operator+()); }
  template <FixedPointArray T> pure auto operator=(T&& x) noexcept -> self_t& requires (
        ((T::decidable and (V == T::V)) or !decidable) and (B == T::b) and (I == T::i) and std::is_same_v<S, typename T::s>);
  // pure auto operator[](std::size_t idx) const noexcept -> scalar_run_t { return scalar_run_t{internal[idx]}; }
  pure auto operator[](std::size_t idx) const noexcept -> op<FixedString{"Index"}, self_t, std::size_t> {
    return {*this, idx};
  }
  pure auto begin() const noexcept -> decltype(auto) { return internal.begin(); }
  pure auto end() const noexcept -> decltype(auto) { return internal.end(); }
  pure auto r2() const noexcept -> op<FixedString{"RadiusSquared"}, self_t> { return {*this}; }
};

template <std::size_t N, u8 B, u8 I, typename S, std::array<bool, N> D, std::array<cint<byte_ceil(B), S>, N> V>
requires (all(D) or none(V))
template <FixedPointArray T> pure auto a<N, B, I, S, D, V>::operator=(T&& x) noexcept -> self_t& requires (
      ((T::decidable and (V == T::V)) or !decidable) and (B == T::b) and (I == T::i) and std::is_same_v<S, typename T::s>)  //
{
  internal = +x;
  return *this;
}

// all overloadable operators: https://en.cppreference.com/w/cpp/language/operators

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define UNARY_PREFIX_OP(SYMBOL, NAME)                                                                                         \
  template <FixedPoint T> pure auto operator SYMBOL(T CONST_IF_RELEASE& x) noexcept -> op<#NAME, T> { return {x}; }
UNARY_PREFIX_OP(-, UnaryMinus);
UNARY_PREFIX_OP(~, BitwiseNot);
UNARY_PREFIX_OP(!, LogicalNot);
UNARY_PREFIX_OP(++, PreIncrement);
UNARY_PREFIX_OP(--, PreDecrement);
#undef UNARY_PREFIX_OP

// TODO(wrsturgeon): call and subscript operators

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define BINARY_OP(SYMBOL, NAME)                                                                                               \
  template <FixedPoint A, typename B>                                                                                         \
  pure auto operator SYMBOL(A CONST_IF_RELEASE& a, B CONST_IF_RELEASE& b) noexcept -> op<#NAME, A, B> {                       \
    return {a, b};                                                                                                            \
  }
BINARY_OP(+, Sum);
BINARY_OP(-, Difference);
BINARY_OP(*, Product);
BINARY_OP(/, Quotient);
BINARY_OP(%, Remainder);
BINARY_OP(^, BitwiseXor);
BINARY_OP(&, BitwiseAnd);
BINARY_OP(|, BitwiseOr);
BINARY_OP(>, GreaterThan);
BINARY_OP(<, LessThan);
BINARY_OP(<<, LeftShift);
BINARY_OP(>>, RightShift);
BINARY_OP(==, Equality);
BINARY_OP(!=, Inequality);
BINARY_OP(<=, LessThanOrEqual);
BINARY_OP(>=, GreaterThanOrEqual);
BINARY_OP(<=>, SpaceshipComparison);
BINARY_OP(&&, LogicalAnd);
BINARY_OP(||, LogicalOr);
#undef BINARY_OP

// // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// #define BINARY_OP_DISCARD(SYMBOL, NAME) \
//   template <FixedPoint A, typename B> INLINE auto operator SYMBOL(A& a, B CONST_IF_RELEASE& b) noexcept -> op<#NAME, A, B> {
//   \
//     return {a, b}; \
//   } \
//   template <FixedPoint A, typename B> INLINE auto operator SYMBOL(A& a, B&& b) noexcept -> op<#NAME, A, B> { \
//     return {a, std::move(b)}; \
//   }
// BINARY_OP_DISCARD(+=, SumAssign);
// BINARY_OP_DISCARD(-=, DifferenceAssign);
// BINARY_OP_DISCARD(*=, ProductAssign);
// BINARY_OP_DISCARD(/=, QuotientAssign);
// BINARY_OP_DISCARD(%=, RemainderAssign);
// BINARY_OP_DISCARD(^=, BitwiseXorAssign);
// BINARY_OP_DISCARD(&=, BitwiseAndAssign);
// BINARY_OP_DISCARD(|=, BitwiseOrAssign);
// BINARY_OP_DISCARD(>>=, RightShiftAssign);
// BINARY_OP_DISCARD(<<=, LeftShiftAssign);
// #undef BINARY_OP_DISCARD

#ifndef NDEBUG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif
template <FixedString Name, FixedPoint T1, typename... Tn> struct op {  // NOLINT(altera-struct-pack-align)
  using argv_t = std::tuple<std::decay_t<T1>, std::decay_t<Tn>...>;
  template <std::size_t j> using arg_t = std::tuple_element_t<j, argv_t>;
  static constexpr u8 b = get_b<T1>;
  static constexpr u8 i = get_i<T1>;
  static constexpr u8 f = get_f<T1>;
  static constexpr bool s = get_s<T1>;
  static constexpr bool decidable = T1::decidable and (Tn::decidable and ...);
  using int_t = typename T1::int_t;
  using rtn_t = typename T1::rtn_t;
 private:
  argv_t argv;  // NOLINT(misc-non-private-member-variables-in-classes)
  template <std::size_t j> pure auto arg() const noexcept -> typename arg_t<j>::rtn_t { return +std::get<j>(argv); }
#ifndef NDEBUG
  bool used = false;
#endif
 public:
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  INLINE op(T1&& x, Tn&&... y) noexcept : argv{std::forward<T1>(x), std::forward<Tn>(y)...} {}
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  INLINE op(T1 const& x, Tn const&... y) noexcept : argv{x, y...} {}
  pure auto operator+() CONST_IF_RELEASE noexcept -> rtn_t;
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  pure operator rtn_t() CONST_IF_RELEASE noexcept { return +*this; }
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  pure operator bool() CONST_IF_RELEASE noexcept { return +*this; }
};
#ifndef NDEBUG
#pragma clang diagnostic pop
#endif

template <FixedString Name, FixedPoint T1, typename... Tn> pure auto
op<Name, T1, Tn...>::operator+() CONST_IF_RELEASE noexcept -> rtn_t {
#ifndef NDEBUG
  assert(!used);  // Huge waste: lazily defined expression evaluated multiple times independently. Evaluate it ahead of time.
  used = true;
#endif
  if constexpr ((Name == FixedString{"Sum"}) || (Name == FixedString{"Difference"})) {
    static constexpr auto ls = arg_t<1>::f - arg_t<0>::f;
    auto normalized = ifc<(ls < 0)>(arg<1>() >> -ls, arg<1>() << ls);
    return ifc<Name == FixedString{"Sum"}>(arg<0>() + normalized, arg<0>() - normalized);
  }
  assert(false);  // unimplemented
  return rtn_t::zero();
}

// template <FixedPoint T1, FixedPoint T2> pure auto
// op<FixedString{"Sum"}, T1, T2>::operator+() const noexcept -> typename op<FixedString{"Sum"}, T1, T2>::rtn_t {
//   static constexpr auto ls = get_f<T2>::value - get_f<T1>::value;
//   auto normalized = ifc<(ls < 0)>(+arg<1>(x) >> -ls, +arg<1>(x) << ls);
//   return static_cast<decltype(+arg(x))>((+arg(x)) + normalized);
// }

// template <FixedPoint T1, FixedPoint T2> pure auto
// operator+(op<FixedString{"Difference"}, T1, T2> const& x) noexcept -> decltype(+arg(x)) {
//   static constexpr auto ls = get_f<T2>::value - get_f<T1>::value;
//   auto normalized = ifc<(ls < 0)>(+arg<1>(x) >> -ls, +arg<1>(x) << ls);
//   return static_cast<decltype(+arg(x))>((+arg(x)) - normalized);
// }

// template <FixedPoint T1, FixedPoint T2> pure auto
// operator+(op<FixedString{"Product"}, T1, T2> const& x) noexcept -> decltype(+arg(x)) {
//   assert((  // no overflow
//         ((std::size_t(+arg(x)) * std::size_t(+arg<1>(x))) >> T2::f) ==
//         cint<byte_ceil(T1::b), std::conditional_t<T1::s, signed, unsigned>>(((+arg(x)) * (+arg<1>(x))) >> T2::f)));
//   return ((+arg(x)) * (+arg<1>(x))) >> T2::f;
// }

// template <FixedPoint T1, FixedPoint T2> pure auto
// operator+(op<FixedString{"Quotient"}, T1, T2> const& x) noexcept -> decltype(+arg(x)) {
//   return static_cast<decltype(+arg(x))>(((+arg(x)) << T2::f) / +arg<1>(x));
// }

// template <FixedPointArray T> pure auto
// operator+(op<FixedString{"RadiusSquared"}, T> const& x) noexcept -> decltype(auto) {
//   using rtn_t = cint<byte_ceil(get_b<T>::value), std::conditional_t<get_s<T>::value, signed, unsigned>>;
//   return std::accumulate((+arg(x)).begin(), (+arg(x)).end(), 0, [](auto a, auto b) {
//     return static_cast<rtn_t>(a + ((b * b) >> get_f<T>::value));
//   });
// }

// template <FixedPoint T> pure auto
// operator+(op<FixedString{"LogicalNot"}, T> const& x) noexcept -> bool {
//   return !+arg(x);
// }

// template <FixedPoint T, std::integral R> pure auto
// operator+(op<FixedString{"RightShift"}, T, R> const& x) noexcept -> decltype(+arg(x)) {
//   return +arg(x) >> arg<1>(x);
// }

}  // namespace fp
