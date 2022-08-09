#include "vision/distortion.hpp"

namespace vision {

Lens::Lens(int16_t radial_, int16_t tangential_x_, int16_t tangential_y_) : radial{radial_}, tangential_x{tangential_x_}, tangential_y{tangential_y_}, inv_lr{128} {}

template <uint32_t diag_sq>
auto Lens::redistort(pxpos_t px) -> pxpos_t {
  uint16_t r2 = util::rshift<util::lgp1(diag_sq) - 16>((px.x * px.x) + (px.y * px.y));  // 16 bits, scaled to account for image size.
  int16_t scaled = (radial * r2 /* 32-bit */) >> 16 /* 16-bit */;                       // Multiplied by the learnable parameter.
  return pxpos_t{
        static_cast<int16_t>(((px.x << 16) + (px.y * tangential_y)) / (65536 + scaled)),
        static_cast<int16_t>(((px.y << 16) + (px.x * tangential_x)) / (65536 + scaled))};
}

}  // namespace vision
