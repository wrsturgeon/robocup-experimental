#pragma once

#include "pxpos.hpp"

#include "eigen.hpp"

#include <cstdint>

namespace vision {

template <pxidx_t w = kImageW, pxidx_t h = kImageH>  //
class NaoImage {
 public:
  static constexpr auto width() -> pxidx_t;
  static constexpr auto height() -> pxidx_t;
 private:
  static constexpr int format = Eigen::StorageOptions::RowMajor;
  using imsize_t = Eigen::Sizes<w, h, 3>;  // NOLINT(readability-magic-numbers)
  using internal_t = Eigen::TensorFixedSize<std::uint8_t, imsize_t, format, pxidx_t>;
  internal_t internal{};  // Underlying Eigen tensor holding pixel values
};

template <pxidx_t w, pxidx_t h>
constexpr auto
NaoImage<w, h>::width() -> pxidx_t {
  return w;
}

template <pxidx_t w, pxidx_t h>
constexpr auto
NaoImage<w, h>::height() -> pxidx_t {
  return h;
}

}  // namespace vision
