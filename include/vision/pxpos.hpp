#pragma once

#include "measure/units.hpp"

#include <cstdint>
#include <iostream>
#include <string>

namespace vision {

using pxidx_t = std::int16_t;

// (0, 0) is the center of the image; expand outward from there
class pxpos_t {
 public:
  explicit pxpos_t(pxidx_t xpos, pxidx_t ypos);
  explicit operator std::string() const;
  friend auto operator<<(std::ostream& os, pxpos_t const& p) -> std::ostream&;
  [[nodiscard]] auto r2() const -> std::uint32_t;

 private:
  pxidx_t const x_;
  pxidx_t const y_;
};

}  // namespace vision
