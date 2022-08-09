#pragma once

#include "util/constexpr-math.hpp"
#include "vision/pxpos.hpp"

#include <math.h>

namespace vision {

/**
 * Class holding parameters representing the idiosyncrasies of any given camera.
 * Can be manually undone & redone to verify current view against internal estimate.
 * TODO: Make these compile-time when not `TRAINING`, configured then saved to disk
 */
class Lens {
public:
  Lens(Lens const&) = delete;
  Lens(int16_t radial_ = 0, int16_t tangential_x_ = 0, int16_t tangential_y_ = 0);
  template <uint32_t diag_sq>
  auto undistort(pxpos_t px) -> pxpos_t;
  template <uint32_t diag_sq>
  auto redistort(pxpos_t px) -> pxpos_t;

protected:
  int16_t radial; // 8 bits used; extra for smooth gradient descent
  int16_t tangential_x;
  int16_t tangential_y;
  // uint16_t zoom = 16384;
  uint16_t inv_lr = 128; // Inverse learning rate: increment over time
};

} // namespace vision
