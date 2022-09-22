#pragma once

#include "util/custom-ints.hpp"
#include "util/fixed-point.hpp"
#include "util/isqrt.hpp"

#include "eigen.hpp"

#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>

//%%%%%%%%%%%%%%%% Pixel positions

using imsize_t = std::uint16_t;

static constexpr std::uint32_t kImageDiagSq = (kImageH * kImageH) + (kImageW * kImageW);
static constexpr auto kImageDiag = isqrt<imsize_t>(kImageDiagSq);
static constexpr std::uint8_t kLgImageDiag = std::bit_width(kImageDiag);

// We need to be able to represent the negative image diagonal to subtract any two points
static constexpr std::uint8_t kPxTBits = kLgImageDiag + 1;
static constexpr std::uint8_t kPxTPw2 = 1 << std::bit_width(kPxTBits);  // e.g. 11 -> 16
static constexpr std::uint8_t kPxTPad = kPxTPw2 - kPxTBits;             // e.g. 11 -> 5

using pxint_t = typename custom_int<kPxTPw2>::signed_t;

class px_t : public fp<kPxTPw2, kPxTPw2 - 1> {
 private:
  using Base = fp<kPxTPw2, kPxTPw2 - 1>;
 public:
  explicit constexpr px_t(pxint_t x) : Base{static_cast<pxint_t>(x << kPxTPad)} {}
  // NOLINTNEXTLINE(hicpp-explicit-conversions,google-explicit-constructor)
  constexpr px_t(Base const& x) : Base{x} {}
  explicit operator pxint_t() const { return static_cast<pxint_t>(internal >> kPxTPad); }
};

// (0, 0) is the center of the image; expand outward from there
class px2d {
 public:
  px_t x;  // NOLINT(misc-non-private-member-variables-in-classes)
  px_t y;  // NOLINT(misc-non-private-member-variables-in-classes)
  explicit constexpr px2d(px_t xpos, px_t ypos) : x{xpos}, y{ypos} {}
  explicit constexpr px2d(pxint_t xpos, pxint_t ypos) : x{xpos}, y{ypos} {}
  explicit operator std::string() const;
  [[nodiscard]] auto r2() const -> px_t;
};

auto
operator+(std::string const& s, px2d const& p) -> std::string {
  return s + static_cast<std::string>(p);
}

px2d::operator std::string() const { return std::string{'('} + x + "x, " + y + "y)"; }

auto
px2d::r2() const -> px_t {
  return ((x * x) + (y * y));
}

//%%%%%%%%%%%%%%%% Physical distance

static constexpr auto kMM2M = 0.001F;

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
  explicit constexpr ds_t(std::int16_t mm);
  [[nodiscard]] auto mm() const -> float { return ldexpf(internal, -lc); }
  [[nodiscard]] auto meters() const -> float { return mm() * kMM2M; }
  explicit operator std::string() const;
  friend auto operator<<(std::ostream& os, ds_t const& p) -> std::ostream&;
};

class ds2d {
 public:
  explicit constexpr ds2d(std::int16_t x_mm, std::int16_t y_mm);
  explicit operator std::string() const;
 private:
  ds_t x;
  ds_t y;
};

constexpr ds_t::ds_t(std::int16_t mm) : internal{static_cast<std::int16_t>(mm << lc)} {
#ifndef NDEBUG
  if ((internal >> lc) != mm) { throw std::overflow_error("ds_t overflow"); }
#endif
}

ds_t::operator std::string() const {
  return std::to_string(static_cast<float>(internal >> lc) * kMM2M) + 'm';
}

auto
operator<<(std::ostream& os, ds_t const& p) -> std::ostream& {
  return os << static_cast<std::string>(p);
}

constexpr ds2d::ds2d(std::int16_t x_mm, std::int16_t y_mm) : x{x_mm}, y{y_mm} {}

ds2d::operator std::string() const {
  return '(' + static_cast<std::string>(x) + " x, " + static_cast<std::string>(y) + " y)";
}

auto
operator<<(std::ostream& os, ds2d const& p) -> std::ostream& {
  return os << static_cast<std::string>(p);
}

template <imsize_t w, imsize_t h>
using Array = Eigen::Array<std::uint8_t, h, w, ((w == 1) ? Eigen::ColMajor : Eigen::RowMajor)>;
