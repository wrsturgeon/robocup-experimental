#if VISION_ENABLED
#ifndef VISION_DISTORTION_HPP_
#define VISION_DISTORTION_HPP_

#include <vision/image_api.hpp>

#include <math.h>

#include <measure/units.hpp>        // idx_t
#include <util/constexpr_math.hpp>  // rshift
#include <vision/pxpos.hpp>         // pxpos_t

namespace vision {



/**
 * Class holding parameters representing the idiosyncrasies of any given camera.
 * Can be manually undone & redone to verify current view against internal estimate.
 * TODO: Make these compile-time when not `TRAINING`, configured then saved to disk
 */
class Lens {
public:
  Lens(Lens const&) = delete;
  Lens(int16_t radial = 0, int16_t tangential_x = 0, int16_t tangential_y = 0) :
        radial{radial}, tangential_x{tangential_x}, tangential_y{tangential_y}, inv_lr{128} {}
  template <uint32_t diag_sq> MEMBER_INLINE pxpos_t undistort(pxpos_t px);
  template <uint32_t diag_sq> MEMBER_INLINE pxpos_t redistort(pxpos_t px);
protected:
  int16_t radial; // 8 bits used; extra for smooth gradient descent
  int16_t tangential_x;
  int16_t tangential_y;
  // uint16_t zoom = 16384;
  uint16_t inv_lr = 128; // Inverse learning rate: increment over time
};



/**
 * Pixel position if taken on an ideal camera -> Actual pixel position (estimated)
 * Fitzgibbon's division model:
 *   x' = x / (1 + radial * x^2)
 */
template <uint32_t diag_sq>
MEMBER_INLINE pxpos_t Lens::redistort(pxpos_t px) {
  uint16_t r2 = util::rshift<util::lgp1(diag_sq) - 16>((px.x * px.x) + (px.y * px.y)); // 16 bits, scaled to account for image size.
  int16_t scaled = (radial * r2 /* 32-bit */) >> 16 /* 16-bit */; // Multiplied by the learnable parameter.
  return pxpos_t{
        static_cast<int16_t>(((px.x << 16) + (px.y * tangential_y)) / (65536 + scaled)),
        static_cast<int16_t>(((px.y << 16) + (px.x * tangential_x)) / (65536 + scaled))};
}



} // namespace vision

#endif // VISION_DISTORTION_HPP_

#else // VISION_ENABLED
#pragma message("Skipping distortion.hpp; vision module disabled")
#endif // VISION_ENABLED
