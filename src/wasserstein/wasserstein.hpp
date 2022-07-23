#if WASSERSTEIN_ENABLED
#ifndef WASSERSTEIN_WASSERSTEIN_HPP_
#define WASSERSTEIN_WASSERSTEIN_HPP_

#include <vision/image_api.hpp>
#include <vision/pyramid.hpp>

namespace wasserstein {

using vision::NaoImage;
using vision::pxidx_t;
using vision::Pyramid;

template <pxidx_t w, pxidx_t h, uint8_t k>
class Wasserstein : public Pyramid<w, h> {
public:
  void recalculate(NaoImage<w, h> const& image);
};



template <pxidx_t w, pxidx_t h, uint8_t k>
void Wasserstein<w, h, k>::recalculate(NaoImage<w, h> const& im) {
  set_layer<0>(im - k);
  // Then build up...
}



} // namespace wasserstein

#endif // WASSERSTEIN_WASSERSTEIN_HPP_

#else // WASSERSTEIN_ENABLED
#pragma message("Skipping wasserstein.hpp; wasserstein module disabled")
#endif // WASSERSTEIN_ENABLED
