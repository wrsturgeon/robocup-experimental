#if VISION_ENABLED
#ifndef VISION_IMAGE_API_HPP_
#define VISION_IMAGE_API_HPP_

#include <stdint.h>

#include "eigen.hpp"

#include "vision/distortion.hpp"
#include "vision/pxpos.hpp"

#if SDL_ENABLED
#include "sdl/window.hpp"
#endif // SDL_ENABLED

namespace vision {



template <pxidx_t w = IMAGE_W, pxidx_t h = IMAGE_H>
class NaoImage {
public:
  NaoImage(NaoImage const&) = delete;
  NaoImage() : internal{} {}
  INLINE constexpr pxidx_t width() { return w; }
  INLINE constexpr pxidx_t height() { return h; }
#if SDL_ENABLED
  MEMBER_INLINE void popup() const { sdl::Window{w, h}.display(*this, true); }
  MEMBER_INLINE operator SDL_Surface*() const;
#endif // SDL_ENABLED
protected:
  static constexpr int format = Eigen::StorageOptions::RowMajor;
  using imsize_t = Eigen::Sizes<w, h, 3>;
  using internal_t = Eigen::TensorFixedSize<uint8_t, imsize_t, format, pxidx_t>;
  internal_t internal; // Underlying Eigen tensor holding pixel values
};

#if SDL_ENABLED
template <pxidx_t w, pxidx_t h>
MEMBER_INLINE NaoImage<w, h>::operator SDL_Surface*() const {
  return SDL_CreateRGBSurfaceWithFormatFrom(
        const_cast<typename internal_t::Scalar*>(internal.data()),
        w, h, 24, 3 * w, SDL_PIXELFORMAT_RGB24);
}
#endif // SDL_ENABLED



} // namespace vision

#endif // VISION_IMAGE_API_HPP_

#else // VISION_ENABLED
#pragma message("Skipping image_api.hpp; vision module disabled")
#endif // VISION_ENABLED
