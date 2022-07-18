#ifndef VISION_DISTORTION_HPP_
#define VISION_DISTORTION_HPP_

#include <math.h>

#include <measurement/units.hpp>    // idx_t
#include <util/constexpr_math.hpp>  // rshift
#include <util/specifiers.hpp>      // INLINE
#include <vision/image_api.hpp>     // int16_t



/**
 * Class holding parameters representing the idiosyncrasies of any given camera.
 * Can be manually undone & redone to verify current view against internal estimate.
 * TODO: Make these compile-time when not `TRAINING`, configured then saved to disk
 */
class Lens {
public:
  template <uint32_t diag_sq> INLINE pxpos_t undistort(pxpos_t px);
  template <uint32_t diag_sq> INLINE pxpos_t redistort(pxpos_t px);
protected:
  int16_t radial = 0; // 8 bits used; extra for smooth gradient descent
  int16_t tangential_x = 0;
  int16_t tangential_y = 0;
  // uint16_t zoom = 16384;
  uint16_t inv_lr = 128; // Inverse learning rate: increment s.t. lr=0 over time
};



/**
 * Pixel position if taken on an ideal camera -> Actual pixel position (estimated)
 * Fitzgibbon's division model:
 *   x' = x / (1 + radial * x^2)
 */
template <uint32_t diag_sq>
INLINE pxpos_t Lens::redistort(pxpos_t px) {
  uint16_t r2 = rshift<lg(diag_sq) - 16>((px.x * px.x) + (px.y * px.y)); // 16 bits, scaled to account for image size.
  int16_t scaled = (radial * r2 /* 32-bit */) >> 16 /* 16-bit */; // Multiplied by the learnable parameter.
  return pxpos_t{
        static_cast<int16_t>(((px.x << 16) + (px.y * tangential_y)) / (65536 + scaled)),
        static_cast<int16_t>(((px.y << 16) + (px.x * tangential_x)) / (65536 + scaled))};
}



#endif // VISION_DISTORTION_HPP_
