#include <options.hpp>
#if VISION_ENABLED
#ifndef VISION_IMAGE_API_HPP_
#define VISION_IMAGE_API_HPP_

#include <sdl/window.hpp>

#include <stdint.h>

#include <eigen.hpp>

#include <util/specifiers.hpp>

namespace vision {

using pxidx_t = int16_t;

// {0, 0} is the center of the image; expand outward from there
class pxpos_t {
public:
  pxpos_t(pxpos_t const&) = delete;
  explicit pxpos_t(pxidx_t x = 0, pxidx_t y = 0) : x{x}, y{y} {}
  MEMBER_INLINE operator std::string() const { return '(' +
        std::to_string(x) + "x, " +
        std::to_string(y) + "y)"; }
  pxidx_t const x = 0;
  pxidx_t const y = 0;
  MEMBER_INLINE uint32_t r2() const { return (x * x) + (y * y); }
};

} // Close namespace temporarily for #include

// Relies on pxpos_t
#include <vision/distortion.hpp> // Lens

namespace vision { // Reopen



template <pxidx_t w = IMAGE_W, pxidx_t h = IMAGE_H>
class NaoImage {
public:
  NaoImage(NaoImage const&) = delete;
  MEMBER_INLINE void popup();
  operator SDL_Surface*() { return SDL_CreateRGBSurfaceWithFormatFrom(internal.data(), w, h, 24, 3 * w, SDL_PIXELFORMAT_RGB24); }
protected:
  static constexpr int format = Eigen::StorageOptions::RowMajor;
  using imsize_t = Eigen::Sizes<w, h, 3>;
  using internal_t = Eigen::TensorFixedSize<uint8_t, imsize_t, format, pxidx_t>;
  internal_t internal; // Underlying Eigen tensor holding pixel values
};

template <pxidx_t w = IMAGE_W, pxidx_t h = IMAGE_H>
MEMBER_INLINE void NaoImage<w, h>::popup() {
  sdl::Popup(w, h);
}



} // namespace vision

#endif // VISION_IMAGE_API_HPP_

#endif // VISION_ENABLED
