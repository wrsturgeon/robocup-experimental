#pragma once

#include "fp/fixed-point.hpp"
#include "util/ints.hpp"
#include "util/isqrt.hpp"

#include "eigen.hpp"

#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>

#ifndef NDEBUG
#include <iostream>
#include <string>
#endif  // NDEBUG

//%%%%%%%%%%%%%%%% Pixel positions

using imsize_t = u16;

inline constexpr auto kImageDiag = isqrt<imsize_t>((kImageH * kImageH) + (kImageW * kImageW));
inline constexpr auto kLgImageDiag = static_cast<u8>(std::bit_width(std::bit_ceil(kImageDiag)));

// We need to be able to represent the negative image diagonal to subtract any two points
inline constexpr u8 kPxTBits = kLgImageDiag + 1;
inline constexpr u8 kPxTPw2 = std::bit_ceil(kPxTBits);  // e.g. 11 -> 16

using pxint_t = cint<kPxTPw2, signed>;
using px_t = fp::t<kPxTPw2, kPxTBits, signed>;  // wiggle room for severe distortion
using px2d = px_t::array<2>;

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
  strpure explicit operator std::string() const;
#endif  // NDEBUG
};

class ds2d {
 private:
  ds_t x;
  ds_t y;
 public:
  explicit constexpr ds2d(i16 x_mm, i16 y_mm) noexcept : x{x_mm}, y{y_mm} {}
#ifndef NDEBUG
  strpure explicit operator std::string() const;
#endif  // NDEBUG
};

constexpr ds_t::ds_t(i16 mm) noexcept : internal{static_cast<i16>(mm << lc)} { assert((internal >> lc) == mm); }

#ifndef NDEBUG

strpure ds_t::operator std::string() const { return std::to_string(static_cast<float>(internal >> lc) * kMM2M) + 'm'; }

strpure static auto operator+(std::string const& s, ds_t const& p) -> std::string { return s + static_cast<std::string>(p); }

// strpure static auto operator<<(std::ostream& os, ds_t const& p) -> std::ostream& { return os << static_cast<std::string>(p);
// }

strpure ds2d::operator std::string() const { return std::string{'('} + x + " x, " + y + " y)"; }

// strpure static auto operator+(std::string const& s, ds2d const& p) -> std::string { return s + static_cast<std::string>(p);
// }

// strpure static auto operator<<(std::ostream& os, ds2d const& p) -> std::ostream& { return os << static_cast<std::string>(p);
// }

#endif  // NDEBUG

template <imsize_t W, imsize_t H>
using Array = Eigen::Array<u8, H, W, ((W == 1) ? Eigen::ColMajor : Eigen::RowMajor)>;
using ChannelArray = Array<kImageW, kImageH>;
using ChannelMap = Eigen::Map<ChannelArray>;

template <imsize_t W, imsize_t H, imsize_t C>
using Tensor = Eigen::TensorFixedSize<u8, Eigen::Sizes<C, H, W>, Eigen::RowMajor>;
template <imsize_t C>
using ImageTensor = Tensor<kImageW, kImageH, C>;
template <imsize_t C>
using ImageMap = Eigen::TensorMap<ImageTensor<C>>;
