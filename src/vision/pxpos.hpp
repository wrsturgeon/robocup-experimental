#pragma once

#include "measure/units.hpp"

#include <cstdint>
#include <iostream>
#include <string>

namespace vision {

using pxidx_t = int16_t;

// Forward declaration to friend below
class Lens;

// (0, 0) is the center of the image; expand outward from there
class pxpos_t {
public:
  explicit pxpos_t(pxidx_t, pxidx_t);
  explicit operator std::string() const;
  friend auto operator<<(std::ostream&, pxpos_t const&) -> std::ostream&;
  [[nodiscard]] auto r2() const -> std::uint32_t;
private:
  friend class Lens;
  pxidx_t const x_;
  pxidx_t const y_;
};

pxpos_t::pxpos_t(pxidx_t xpos, pxidx_t ypos) :
      x_{xpos},
      y_{ypos} {}

pxpos_t::operator std::string() const {
  return '(' + std::to_string(x_) + "x, " + std::to_string(y_) + "y)";
}

auto pxpos_t::r2() const -> uint32_t {
  return (
        static_cast<uint32_t>(x_ * x_) +  // overflow-safe
        static_cast<uint32_t>(y_ * y_));
}

auto operator<<(std::ostream& os, pxpos_t const& p) -> std::ostream& {
  return os << static_cast<std::string>(p);
}

}  // namespace vision
