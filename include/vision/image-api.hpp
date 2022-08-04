#ifndef VISION_IMAGE_API_HPP_
#define VISION_IMAGE_API_HPP_

#include "pxpos.hpp"

#include "eigen.hpp"

#include <stdint.h>

namespace vision {

template <pxidx_t w = IMAGE_W, pxidx_t h = IMAGE_H>
class NaoImage {
public:
  NaoImage(NaoImage const&) = delete;
  NaoImage();
  static constexpr pxidx_t width();
  static constexpr pxidx_t height();

protected:
  static constexpr int format = Eigen::StorageOptions::RowMajor;
  using imsize_t = Eigen::Sizes<w, h, 3>;
  using internal_t = Eigen::TensorFixedSize<uint8_t, imsize_t, format, pxidx_t>;
  internal_t internal; // Underlying Eigen tensor holding pixel values
};

} // namespace vision

#endif // VISION_IMAGE_API_HPP_
