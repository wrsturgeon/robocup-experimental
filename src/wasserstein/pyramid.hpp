#ifndef WASSERSTEIN_PYRAMID_HPP_
#define WASSERSTEIN_PYRAMID_HPP_

#include <util/constexpr_math.hpp>
#include <vision/image_api.hpp>

namespace wstn {



template <pxidx_t W, pxidx_t H>
class Pyramid {
public:
protected:
  static constexpr uint8_t n_layers = cm::lgp1((H > W) ? H : W);
  NaoImage const img[n_layers];
};



} // namespace wstn

#endif // WASSERSTEIN_PYRAMID_HPP_
