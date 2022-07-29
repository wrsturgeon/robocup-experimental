#ifndef VISION_DISTORTION_HPP_
#define VISION_DISTORTION_HPP_

#include <math.h>

#include "measure/units.hpp"
#include "util/constexpr_math.hpp"
#include "vision/pxpos.hpp"
#include "vision/image_api.hpp"

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
  template <uint32_t diag_sq> MEMBER_INLINE pxpos_t undistort(pxpos_t px);
  template <uint32_t diag_sq> MEMBER_INLINE pxpos_t redistort(pxpos_t px);
protected:
  int16_t radial; // 8 bits used; extra for smooth gradient descent
  int16_t tangential_x;
  int16_t tangential_y;
  // uint16_t zoom = 16384;
  uint16_t inv_lr = 128; // Inverse learning rate: increment over time
};



} // namespace vision

#endif // VISION_DISTORTION_HPP_
