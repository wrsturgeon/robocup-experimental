#if VISION_ENABLED
#ifndef VISION_WASSERSTEIN_HPP_
#define VISION_WASSERSTEIN_HPP_

#include <eigen.hpp>

#include <vision/image_api.hpp>

namespace vision {



// Naive method.
// Likely won't use any further optimizations like from Eigen,
// but then again I don't know if there are any to be made here.
// See https://stackoverflow.com/questions/16283000
template <pxidx_t w, pxidx_t h>
class Wasserstein {
public:
  Wasserstein() : level{} {}
  void init(NaoImage<w, h> const& im);
protected:
  static constexpr int format = Eigen::StorageOptions::RowMajor;
  using matrix_t = Eigen::Matrix<uint8_t, w, h, format>;
  static constexpr uint8_t n_levels = util::lgp1((h > w) ? h : w);
  matrix_t const level[n_levels];
};



} // namespace vision

#endif // VISION_WASSERSTEIN_HPP_

#else // VISION_ENABLED
#pragma message("Skipping wasserstein.hpp; vision module disabled")
#endif // VISION_ENABLED
