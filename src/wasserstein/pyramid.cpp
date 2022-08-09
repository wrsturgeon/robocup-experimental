#include "wasserstein/pyramid.hpp"

namespace wasserstein {

using Eigen::seqN;
using Eigen::placeholders::all;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-length-array"

inline static constexpr size_t
pyrsize(vision::pxidx_t w, vision::pxidx_t h) {
  return (w && h) ? static_cast<size_t>(w * h) + pyrsize(w >> 1, h >> 1) : 0;
}

template <vision::pxidx_t w, vision::pxidx_t h>
Pyramid<w, h>::Pyramid(uint8_t src[h][w]) : Pyramid{&src[0][0]} {}

template <vision::pxidx_t w, vision::pxidx_t h>
Pyramid<w, h>::Pyramid(vision::NaoImage<w, h> const& src) : Pyramid{src.internal.data()} {}

template <vision::pxidx_t w, vision::pxidx_t h>
uint8_t&
Pyramid<w, h>::operator()(vision::pxidx_t x, vision::pxidx_t y) {
  return _array[y][x];
}

template <vision::pxidx_t w, vision::pxidx_t h>
typename Pyramid<w, h>::up_t&
Pyramid<w, h>::up() {
  return *reinterpret_cast<up_t*>(_up_raw);
  // The coolest thing is that it doesn't even matter if we call up() one or two or n times too many--
  // it'll still return the same 0-element Pyramid!
  // We just need some kind of minimal (preferably compile-time) bounds checking in public methods
  // And keep in mind that 0-element arrays are a compiler extension
}

template <vision::pxidx_t w, vision::pxidx_t h>
Pyramid<w, h>::Pyramid(uint8_t* const __restrict src) {
// Private constructor: guaranteed that by this point, src points to valid (w, h) uint8_t data
#define PYRAMID_ERROR "Pyramid class must have only 2 allocated members: _array & _up_raw"
  static_assert(!sizeof(Pyramid<0, 0>), PYRAMID_ERROR);
  static_assert(sizeof(Pyramid<1, 1>) == 1, PYRAMID_ERROR);
  // memcpy(_array, src, n_px);
  // build();
  EigenMap<w, h> input_map{src};
  EigenMap<w, h> internal_map{&_array[0][0]};
  internal_map = ~input_map; // 255 -> 0, 254 -> 1, ..., 0 -> 255
  build_eigen(internal_map);
}

template <vision::pxidx_t w, vision::pxidx_t h>
void Pyramid<w, h>::build_manual() {
  // Assumes we've already filled `_array` with valid image data

  static_assert(sizeof(Pyramid<w, h>) == pyrsize(w, h), PYRAMID_ERROR);

  // NOTE: AN INFINITE CONSTRUCTOR LOOP DOES *NOT* THROW A COMPILE-TIME ERROR
  // Need to verify that we have > 0 pixels to fill, else infinite loop of constructing nothing
  if ((!half_w) || (!half_h)) {
    return;
  }

  // Allocating for use in loops
  vision::pxidx_t twice;
  uint8_t a, b;

  // TODO: randomize shift when odd size

  // One dimension at a time: first half the width
  uint8_t tmp[h][half_w];
  for (vision::pxidx_t y = 0; y < h; ++y) {
    for (vision::pxidx_t x = 0; x < half_w; ++x) {
      twice = static_cast<vision::pxidx_t>(x << 1); // TODO: verify no overflow
      a = _array[y][twice];
      b = _array[y][twice | 1];
      tmp[y][x] = ((a >> 2) > (b >> 2)) ? a & ~1 : b | 1; // Set a bit to represent left/right provenience (thx Sydney)
    }
  }

  // And half that, directly into the next layer
  Pyramid<half_w, half_h>& dst = up();
  for (vision::pxidx_t y = 0; y < half_h; ++y) {
    for (vision::pxidx_t x = 0; x < half_w; ++x) {
      twice = static_cast<vision::pxidx_t>(y << 1); // TODO: verify no overflow
      a = tmp[twice][x];
      b = tmp[twice | 1][x];
      dst(x, y) = ((a >> 2) > (b >> 2)) ? a & ~2 : b | 2;
    }
  }

  dst.build_manual();
}

template <vision::pxidx_t w, vision::pxidx_t h>
void Pyramid<w, h>::build_eigen(EigenMap<w, h> const& lower_map) {
  static_assert(half_w && half_h, "Pyramid level to be constructed must have at least one pixel");
  static_assert(sizeof(Pyramid<w, h>) == pyrsize(w, h), PYRAMID_ERROR);
  uint8_t i0;
  Pyramid<half_w, half_h>& next = up();

  // Eigen has some obsessive beef with things that look like column vectors but aren't
  if constexpr (half_w == 1) {
    Eigen::Map<Eigen::Vector<uint8_t, half_h>> upper_map{&next._array[0][0]};
    if constexpr (w & 1) {
      i0 = rnd::bit();
    } else {
      i0 = 0;
    }
    auto tmp = (lower_map(all, seqN(i0, half_w, 2)) >> 1) + (lower_map(all, seqN(i0 + 1, half_w, 2)) >> 1);
    if constexpr (h & 1) {
      i0 = rnd::bit();
    } else {
      i0 = 0;
    }
    upper_map = (tmp(seqN(i0, half_h, 2), all) >> 1) + (tmp(seqN(i0 + 1, half_h, 2), all) >> 1);
  } else { // Literally anything else
    EigenMap<half_w, half_h> upper_map{&next._array[0][0]};
    if constexpr (w & 1) {
      i0 = rnd::bit();
    } else {
      i0 = 0;
    }
    auto tmp = (lower_map(all, seqN(i0, half_w, 2)) >> 1) + (lower_map(all, seqN(i0 + 1, half_w, 2)) >> 1);
    if constexpr (h & 1) {
      i0 = rnd::bit();
    } else {
      i0 = 0;
    }
    upper_map = (tmp(seqN(i0, half_h, 2), all) >> 1) + (tmp(seqN(i0 + 1, half_h, 2), all) >> 1);
    if constexpr (half_h != 1) {
      next.build_eigen(upper_map);
    }
  }
}

#pragma clang diagnostic pop

} // namespace wasserstein
