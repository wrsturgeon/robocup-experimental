#include "vision/image_api.hpp"

namespace vision {



template <pxidx_t w, pxidx_t h> NaoImage<w, h>::NaoImage() : internal{} {}

template <pxidx_t w, pxidx_t h> INLINE constexpr pxidx_t NaoImage<w, h>::width() { return w; }
template <pxidx_t w, pxidx_t h> INLINE constexpr pxidx_t NaoImage<w, h>::height() { return h; }

#if SDL_ENABLED
template <pxidx_t w, pxidx_t h> MEMBER_INLINE void NaoImage<w, h>::popup() const { sdl::Window{w, h}.display(*this, true); }

template <pxidx_t w, pxidx_t h>
MEMBER_INLINE NaoImage<w, h>::operator SDL_Surface*() const {
  return SDL_CreateRGBSurfaceWithFormatFrom(
        const_cast<typename internal_t::Scalar*>(internal.data()),
        w, h, 24, 3 * w, SDL_PIXELFORMAT_RGB24);
}
#endif // SDL_ENABLED



} // namespace vision
