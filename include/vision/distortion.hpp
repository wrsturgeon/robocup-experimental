#pragma once

#include "vision/pxpos.hpp"

#include <cstdint>

namespace vision {

/**
 * Class holding parameters representing the idiosyncrasies of any given camera.
 * Can be manually undone & redone to verify current view against internal estimate.
 * TODO: Make these compile-time when not `TRAINING`, configured then saved to disk
 */
class Lens {
 public:
  explicit Lens(std::int16_t, std::int16_t, std::int16_t);
  template <std::uint32_t>
  auto undistort(pxpos_t) -> pxpos_t;
  template <std::uint32_t>
  auto redistort(pxpos_t) -> pxpos_t;

 private:
  std::int16_t radial;  // 8 bits used; extra for smooth gradient descent
  std::int16_t tangential_x;
  std::int16_t tangential_y;
  std::uint16_t inv_lr;  // Inverse learning rate: increment over time
};

}  // namespace vision
