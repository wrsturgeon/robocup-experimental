#pragma once

#include <stdint.h>

#include <iostream>
#include <string>

#include "measure/units.hpp"

namespace vision {

using pxidx_t = int16_t;

// (0, 0) is the center of the image; expand outward from there
class pxpos_t {
 public:
  pxpos_t(pxpos_t const&) = delete;
  explicit pxpos_t(pxidx_t x_ = 0, pxidx_t y_ = 0);
  operator std::string() const;
  friend std::ostream& operator<<(std::ostream& os, pxpos_t const& p);
  pxidx_t const x = 0;
  pxidx_t const y = 0;
  uint32_t r2() const;
};

}  // namespace vision
