#include "vision/image-api.hpp"

namespace vision {

template <pxidx_t w, pxidx_t h>
NaoImage<w, h>::NaoImage() : internal{} {}

template <pxidx_t w, pxidx_t h>
consteval auto NaoImage<w, h>::width() -> pxidx_t {
  return w;
}

template <pxidx_t w, pxidx_t h>
consteval auto NaoImage<w, h>::height() -> pxidx_t {
  return h;
}

}  // namespace vision
