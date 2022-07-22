#if VISION_ENABLED
#ifndef VISION_PYRAMID_HPP_
#define VISION_PYRAMID_HPP_

#include <util/constexpr_math.hpp>
#include <vision/image_api.hpp>

namespace vision {



template <pxidx_t w, pxidx_t h>
class Pyramid {
// public:
protected:
  static constexpr uint8_t n_layers = util::lgp1((h > w) ? h : w);
  NaoImage<w, h> const im[n_layers];
};



} // namespace vision

#endif // VISION_PYRAMID_HPP_

#else // VISION_ENABLED
#pragma message("Skipping pyramid.hpp; vision module disabled")
#endif // VISION_ENABLED
