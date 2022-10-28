#pragma once

#include "fp/fixed-point.hpp"
#include "util/byte-ceil.hpp"
#include "util/ints.hpp"
#include "util/isqrt.hpp"

#include "eigen.hpp"

#include <bit>
#include <cmath>
#include <cstdint>
#include <type_traits>

#ifndef NDEBUG
#include <iostream>
#include <string>
#endif  // NDEBUG

//%%%%%%%%%%%%%%%% Physical distance
// TODO(wrsturgeon): unfuck this (use fp<...>)

inline constexpr auto kMM2M = 0.001F;

/**
 * Half-millimeters from the center of the field.
 * Integral type, not float, so try not to add vanishingly small amounts.
 * 1 meter := 2000 ds_t
 * Corner-to-corner field of play ~= 10.8m ~= 21,634 ds_t.
 * This type is just enough to represent negative-all-the-way-corner-to-corner,
 * which is exactly what we need to compare any two positions on the field.
 */
class ds_t {
 private:
  static constexpr u8 lc = 1;  // lg(conversion to mm)
  // TODO(wrsturgeon): verify compile-time that
  // this fits within a SIGNED-size (i16) array
  i16 internal;
 public:
  // Purposefully no integer conversion ops: must intentionally take ds_t
  explicit constexpr ds_t(i16 mm) noexcept;
  pure auto mm() const -> float { return ldexpf(internal, -lc); }
  pure auto meters() const -> float { return mm() * kMM2M; }
#ifndef NDEBUG
  impure explicit operator std::string() const;
#endif  // NDEBUG
};

class ds2d {
 private:
  ds_t x;
  ds_t y;
 public:
  explicit constexpr ds2d(i16 x_mm, i16 y_mm) noexcept : x{x_mm}, y{y_mm} {}
#ifndef NDEBUG
  impure explicit operator std::string() const;
#endif  // NDEBUG
};

constexpr ds_t::ds_t(i16 mm) noexcept : internal{static_cast<i16>(mm << lc)} { assert((internal >> lc) == mm); }

#ifndef NDEBUG

impure ds_t::operator std::string() const { return std::to_string(static_cast<float>(internal >> lc) * kMM2M) + 'm'; }

impure static auto
operator+(std::string const& s, ds_t const& p) -> std::string {
  return s + static_cast<std::string>(p);
}

// impure static auto operator<<(std::ostream& os, ds_t const& p) -> std::ostream& { return os << static_cast<std::string>(p);
// }

impure ds2d::operator std::string() const { return std::string{'('} + x + " x, " + y + " y)"; }

// impure static auto operator+(std::string const& s, ds2d const& p) -> std::string { return s + static_cast<std::string>(p);
// }

// impure static auto operator<<(std::ostream& os, ds2d const& p) -> std::ostream& { return os << static_cast<std::string>(p);
// }

#endif  // NDEBUG

template <int H, int W> inline constexpr auto RowMajor = ((W == 1) ? Eigen::ColMajor : Eigen::RowMajor);
template <int H, int W> inline constexpr auto ColMajor = ((H == 1) ? Eigen::RowMajor : Eigen::ColMajor);

template <int H, int W, typename T = u8> requires ((W > 0) and (H > 0))
using Array = Eigen::Array<T, H, W, RowMajor<H, W>>;
template <int N, typename T = u8> using Vector = Array<N, 1, T>;  // Column vector

template <int H, int W, int C, typename T = u8>
using Tensor = Eigen::TensorFixedSize<T, Eigen::Sizes<H, W, C>, Eigen::RowMajor>;
template <int H = kImageH, int W = kImageW, int C = 3, typename T = u8> using ImageTensor = Tensor<H, W, C, T>;
template <int H = kImageH, int W = kImageW, int C = 3, typename T = u8> requires (C > 1)
using ImageMap = Eigen::Map<Array<H * W, C, T>>;
template <int H = kImageH, int W = kImageW, typename T = u8> using ChannelMap = Eigen::Map<Array<H, W, T>, RowMajor<H, W>>;

template <typename T> concept EigenExpressible = std::is_base_of_v<Eigen::EigenBase<T>, T> or
      std::is_base_of_v<Eigen::EigenBase<typename T::Derived>, typename T::Derived>;
template <typename T, int H, int W, typename t = u8>
concept ArrayExpressible = EigenExpressible<T> && std::is_convertible_v<T, Array<H, W, t>>;
template <typename T, int H, int W, int C, typename t = u8>
concept TensorExpressible = (EigenExpressible<T> && std::is_convertible_v<T, Tensor<H, W, C, t>>) or
      ((C == 1) && ArrayExpressible<T, H, W, t>);
