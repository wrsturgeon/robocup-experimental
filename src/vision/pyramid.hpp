#include <options.hpp>
#if VISION_ENABLED
#ifndef VISION_PYRAMID_HPP_
#define VISION_PYRAMID_HPP_

#include <vision/image_api.hpp>
#include <util/constexpr_math.hpp>

namespace vision {



template <pxidx_t W, pxidx_t H>
class Pyramid {
// public:
protected:
  static constexpr uint8_t n_layers = util::lgp1((H > W) ? H : W);
  NaoImage const img[n_layers];
};



} // namespace vision

#endif // VISION_PYRAMID_HPP_

#endif // VISION_ENABLED
