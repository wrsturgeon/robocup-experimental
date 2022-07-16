#ifndef VISION_LENS_DISTORTION_HPP_
#define VISION_LENS_DISTORTION_HPP_

#include <math.h>

#include <util/constexpr_math.hpp>
#include <util/specifiers.hpp>
#include <vision/image_api.hpp>



static int16_t LENS_LAMBDA = 0; // 8 bits used; extra 8 for smooth gradient descent

// Bits
// Max abs int16_t = -(1 << 15) = -32768
// That squared = 1073741824 = 1 << 30
// That plus itself: 2147483648 = 1 << 31

// Fitzgibbon's division model: actual pixel position -> pixel position if taken on an ideal camera (estimated)
template <uint32_t diag_sq>
INLINE pxpos_t lens_undistort(pxpos_t pos) {
  uint16_t r2 = rshift<lg(diag_sq) - 16>((pos.x * pos.x) + (pos.y * pos.y)); // 16 bits, scaled to account for image size.
  int8_t scaled = ((LENS_LAMBDA >> 8 /* 8-bit */) * r2 /* now 24-bit */) >> 16 /* 8-bit */;
  return pxpos_t{
        .x = (pos.x << 7) / (128 + scaled),
        .y = (pos.y << 7) / (128 + scaled)};
}

// Fitzgibbon's division model: pixel position if taken on an ideal camera -> actual pixel position (estimated)
INLINE pxpos_t lens_redistort(pxpos_t pos);



#endif // VISION_LENS_DISTORTION_HPP_
