#pragma once

#include "pxpos.hpp"

#include "eigen.hpp"

#include <cstdint>

namespace vision {

template <pxidx_t w = IMAGE_W, pxidx_t h = IMAGE_H>
class NaoImage {
 public:
  NaoImage();
  static constexpr auto width() -> pxidx_t;
  static constexpr auto height() -> pxidx_t;

 private:
  static constexpr int format = Eigen::StorageOptions::RowMajor;
  using imsize_t = Eigen::Sizes<w, h, 3>;
  using internal_t = Eigen::TensorFixedSize<std::uint8_t, imsize_t, format, pxidx_t>;
  internal_t internal;  // Underlying Eigen tensor holding pixel values
};

}  // namespace vision
