#pragma once

#include "measure/units.hpp"

#include <iostream>
#include <stdint.h>
#include <string>

namespace vision {

using pxidx_t = int16_t;

// (0, 0) is the center of the image; expand outward from there
class pxpos_t {
public:
  pxpos_t(pxpos_t const&) = delete;
  explicit pxpos_t(pxidx_t x_ = 0, pxidx_t y_ = 0);
  operator std::string() const;
  friend auto operator<<(std::ostream& os, pxpos_t const& p) -> std::ostream&;
  pxidx_t const x = 0;
  pxidx_t const y = 0;
  auto r2() const -> uint32_t;
};

} // namespace vision
