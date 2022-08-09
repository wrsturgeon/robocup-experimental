#pragma once

#include "pxpos.hpp"

#include "eigen.hpp"

#include <stdint.h>

namespace vision {

template <pxidx_t w = IMAGE_W, pxidx_t h = IMAGE_H>
class NaoImage {
 public:
  NaoImage(NaoImage const&) = delete;
  NaoImage();
  static consteval auto width() -> pxidx_t;
  static consteval auto height() -> pxidx_t;

 protected:
  static constexpr int format = Eigen::StorageOptions::RowMajor;
  using imsize_t = Eigen::Sizes<w, h, 3>;
  using internal_t = Eigen::TensorFixedSize<uint8_t, imsize_t, format, pxidx_t>;
  internal_t internal;  // Underlying Eigen tensor holding pixel values
};

}  // namespace vision
