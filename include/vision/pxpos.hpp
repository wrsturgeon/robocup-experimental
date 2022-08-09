#pragma once

#include "measure/units.hpp"

#include <cstdint>
#include <iostream>
#include <string>

namespace vision {

using pxidx_t = std::int16_t;

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

}  // namespace vision
