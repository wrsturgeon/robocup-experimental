#ifndef VISION_IMAGE_API_HPP_
#define VISION_IMAGE_API_HPP_

#include <stdint.h>
#include <type_traits>

#include <unsupported/Eigen/CXX11/TensorSymmetry>

#include <util/specifiers.hpp> // INLINE



// {0, 0} is the center of the image; expand outward from there
class pxpos_t {
public:
  pxpos_t(pxpos_t const&) = delete;
  explicit pxpos_t(int16_t x = 0, int16_t y = 0) : x{x}, y{y} {}
  int16_t const x = 0;
  int16_t const y = 0;
  uint32_t r2() const { return (x * x) + (y * y); }
};



// Relies on pxpos_t
#include <vision/distortion.hpp> // Lens



class NaoImage {
public:
  NaoImage(NaoImage const&) = delete;
protected:
  static constexpr int format = Eigen::StorageOptions::RowMajor;
  using imsize_t = Eigen::Sizes<1280, 960, 3>;
  using internal_t = Eigen::TensorFixedSize<uint8_t, imsize_t, format, int16_t>;
  internal_t internal; // Underlying Eigen tensor holding pixel values
};



#endif // VISION_IMAGE_API_HPP_
