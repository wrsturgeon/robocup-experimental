#include "vision/image-api.hpp"

namespace vision {

template <pxidx_t w, pxidx_t h>
NaoImage<w, h>::NaoImage() : internal{} {}

template <pxidx_t w, pxidx_t h>
constexpr pxidx_t NaoImage<w, h>::width() {
  return w;
}
template <pxidx_t w, pxidx_t h>
constexpr pxidx_t NaoImage<w, h>::height() {
  return h;
}

} // namespace vision
