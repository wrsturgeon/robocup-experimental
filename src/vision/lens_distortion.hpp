#ifndef VISION_LENS_DISTORTION_HPP_
#define VISION_LENS_DISTORTION_HPP_

#include <math.h>

#include <util/constexpr_math.hpp>
#include <util/specifiers.hpp>
#include <vision/image_api.hpp>



static int16_t LENS_LAMBDA = 256; // 8 bits used; extra 8 for smooth gradient descent

// Bits
// Max abs int16_t = -(1 << 15) = -32768
// That squared = 1073741824 = 1 << 30
// That plus itself: 2147483648 = 1 << 31

/**
 * Actual pixel position -> Pixel position if taken on an ideal camera (estimated)
 * Fitzgibbon's division model:
 *   x' = x / (1 + lambda * x^2)
 */
template <uint32_t diag_sq>
INLINE pxpos_t lens_undistort(pxpos_t pos) {
  uint16_t r2 = rshift<lg(diag_sq) - 16>((pos.x * pos.x) + (pos.y * pos.y)); // 16 bits, scaled to account for image size.
  int16_t scaled = (LENS_LAMBDA * r2 /* 32-bit */) >> 16 /* 16-bit */; // Multiplied by the learnable parameter.
  static constexpr uint32_t one = 1 << 16; // Making sure it's a compile-time constant; see below
  return pxpos_t{
        static_cast<pxidx_t>((pos.x << 16) / (one + scaled)),
        static_cast<pxidx_t>((pos.y << 16) / (one + scaled))};
}



#endif // VISION_LENS_DISTORTION_HPP_