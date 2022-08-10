#pragma once

#include "pxpos.hpp"

#include "eigen.hpp"

#include <cstdint>

namespace vision {

template <pxidx_t w, pxidx_t h>
NaoImage<w, h>::NaoImage() :
      internal{} {}

template <pxidx_t w, pxidx_t h>
constexpr auto NaoImage<w, h>::width() -> pxidx_t {
  return w;
}

template <pxidx_t w, pxidx_t h>
constexpr auto NaoImage<w, h>::height() -> pxidx_t {
  return h;
}

}  // namespace vision
