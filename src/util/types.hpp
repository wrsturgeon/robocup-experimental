#pragma once

#include "util/custom-ints.hpp"

#include "eigen.hpp"

#include <cstdint>

static constexpr std::uint8_t kImageAddressBits = 16;  // e.g. 16 ==> up to 2^16-width images

using dtype = std::uint8_t;  // duh, but just in case

using imsize_t = custom_int<kImageAddressBits>::unsigned_t;
using pxidx_t = custom_int<kImageAddressBits>::signed_t;
using pxr2_t = custom_int<(kImageAddressBits << 1)>::unsigned_t;

// (0, 0) is the center of the image; expand outward from there
class pxpos_t {
 public:
  pxidx_t const x;  // NOLINT(misc-non-private-member-variables-in-classes)
  pxidx_t const y;  // NOLINT(misc-non-private-member-variables-in-classes)
  explicit pxpos_t(pxidx_t xpos, pxidx_t ypos) : x{xpos}, y{ypos} {}
  operator std::string() const;
  [[nodiscard]] auto r2() const -> pxr2_t;
};

pxpos_t::operator std::string() const {
  return '(' + std::to_string(x) + "x, " + std::to_string(y) + "y)";
}

auto
pxpos_t::r2() const -> pxr2_t {
  return (static_cast<pxr2_t>(x * x) + static_cast<pxr2_t>(y * y));
}

template <imsize_t w, imsize_t h>
using Array = Eigen::Array<dtype, h, w, ((w == 1) ? Eigen::ColMajor : Eigen::RowMajor)>;
