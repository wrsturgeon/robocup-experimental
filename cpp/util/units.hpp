#pragma once

#include "fp/fixed-point.hpp"
#include "util/custom-int.hpp"
#include "util/isqrt.hpp"

#include "eigen.hpp"

#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>

//%%%%%%%%%%%%%%%% Pixel positions

using imsize_t = std::uint16_t;

inline constexpr auto kImageDiag = isqrt<imsize_t>((kImageH * kImageH) + (kImageW * kImageW));
inline constexpr auto kLgImageDiag = static_cast<std::uint8_t>(std::bit_width(std::bit_ceil(kImageDiag)));

// We need to be able to represent the negative image diagonal to subtract any two points
inline constexpr std::uint8_t kPxTBits = kLgImageDiag + 1;
inline constexpr std::uint8_t kPxTPw2 = std::bit_ceil(kPxTBits);  // e.g. 11 -> 16

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
  static constexpr std::uint8_t lc = 1;  // lg(conversion to mm)
  // TODO(wrsturgeon): verify compile-time that
  // this fits within a SIGNED-size (std::int16_t) array
  std::int16_t internal;
 public:
  // Purposefully no integer conversion ops: must intentionally take ds_t
  explicit constexpr ds_t(std::int16_t mm) noexcept;
  [[nodiscard]] pure auto mm() const -> float { return ldexpf(internal, -lc); }
  [[nodiscard]] pure auto meters() const -> float { return mm() * kMM2M; }
  [[nodiscard]] pure explicit operator std::string() const;
};

class ds2d {
 private:
  ds_t x;
  ds_t y;
 public:
  explicit constexpr ds2d(std::int16_t x_mm, std::int16_t y_mm) noexcept : x{x_mm}, y{y_mm} {}
  [[nodiscard]] pure explicit operator std::string() const;
};

constexpr ds_t::ds_t(std::int16_t mm) noexcept : internal{static_cast<std::int16_t>(mm << lc)} {
  assert((internal >> lc) == mm);
}

pure ds_t::operator std::string() const { return std::to_string(static_cast<float>(internal >> lc) * kMM2M) + 'm'; }

static auto operator+(std::string const& s, ds_t const& p) -> std::string { return s + static_cast<std::string>(p); }

// static auto operator<<(std::ostream& os, ds_t const& p) -> std::ostream& { return os << static_cast<std::string>(p); }

pure ds2d::operator std::string() const { return std::string{'('} + x + " x, " + y + " y)"; }

// static auto operator+(std::string const& s, ds2d const& p) -> std::string { return s + static_cast<std::string>(p); }

// static auto operator<<(std::ostream& os, ds2d const& p) -> std::ostream& { return os << static_cast<std::string>(p); }

template <imsize_t w, imsize_t h>
using Array = Eigen::Array<std::uint8_t, h, w, ((w == 1) ? Eigen::ColMajor : Eigen::RowMajor)>;
using ImageArray = Array<kImageW, kImageH>;
using ImageMap = Eigen::Map<ImageArray>;
