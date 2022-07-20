#include <options.hpp>
#if VISION_ENABLED
#ifndef VISION_IMAGE_API_HPP_
#define VISION_IMAGE_API_HPP_

#include <stdint.h>

#include <alloca.h>
#include <unsupported/Eigen/CXX11/TensorSymmetry>

#include <options.hpp>     // IMAGE_W/H

namespace vision {

using pxidx_t = int16_t;

// {0, 0} is the center of the image; expand outward from there
class pxpos_t {
public:
  pxpos_t(pxpos_t const&) = delete;
  explicit pxpos_t(pxidx_t x = 0, pxidx_t y = 0) : x{x}, y{y} {}
  operator std::string() const { return '(' +
        std::to_string(x) + "x, " +
        std::to_string(y) + "y)"; }
  pxidx_t const x = 0;
  pxidx_t const y = 0;
  uint32_t r2() const { return (x * x) + (y * y); }
};

} // Close namespace temporarily for #include

// Relies on pxpos_t
#include <vision/distortion.hpp> // Lens

namespace vision { // Reopen



class NaoImage {
public:
  NaoImage(NaoImage const&) = delete;
protected:
  static constexpr int format = Eigen::StorageOptions::RowMajor;
  using imsize_t = Eigen::Sizes<IMAGE_W, IMAGE_H, 3>;
  using internal_t = Eigen::TensorFixedSize<uint8_t, imsize_t, format, pxidx_t>;
  internal_t internal; // Underlying Eigen tensor holding pixel values
};



} // namespace vision

#endif // VISION_IMAGE_API_HPP_

#endif // VISION_ENABLED
