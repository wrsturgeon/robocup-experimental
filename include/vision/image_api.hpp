#if VISION_ENABLED
#ifndef VISION_IMAGE_API_HPP_
#define VISION_IMAGE_API_HPP_

#include <stdint.h>

#include "eigen.hpp"

#include "vision/distortion.hpp"
#include "vision/pxpos.hpp"

namespace vision {



template <pxidx_t w = IMAGE_W, pxidx_t h = IMAGE_H>
class NaoImage {
public:
  NaoImage(NaoImage const&) = delete;
  NaoImage();
  INLINE constexpr pxidx_t width();
  INLINE constexpr pxidx_t height();
protected:
  static constexpr int format = Eigen::StorageOptions::RowMajor;
  using imsize_t = Eigen::Sizes<w, h, 3>;
  using internal_t = Eigen::TensorFixedSize<uint8_t, imsize_t, format, pxidx_t>;
  internal_t internal; // Underlying Eigen tensor holding pixel values
};



} // namespace vision

#endif // VISION_IMAGE_API_HPP_

#else // VISION_ENABLED
#pragma message("Skipping image_api.hpp; vision module disabled")
#endif // VISION_ENABLED
