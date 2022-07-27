#if WASSERSTEIN_ENABLED
#ifndef WASSERSTEIN_PYRAMID_HPP_
#define WASSERSTEIN_PYRAMID_HPP_

#include <iostream>
#include <string.h>

#include "eigen.hpp"

#include "rnd/xoshiro.hpp"
#include "vision/image_api.hpp"

namespace wasserstein {

using imsize_t = uint16_t;

template <imsize_t w, imsize_t h> using EigenMap = Eigen::Map<Eigen::Matrix<uint8_t, h, w, Eigen::RowMajor>>;
using Eigen::placeholders::all;
using Eigen::seqN;



static constexpr size_t pyrsize(imsize_t w, imsize_t h) { return (w && h) ? (w * h) + pyrsize(w >> 1, h >> 1) : 0; }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-length-array"
template <imsize_t w, imsize_t h>
class Pyramid {
protected:
  template <imsize_t w_, imsize_t h_> friend class Pyramid;
  static constexpr uint32_t n_px = w * h;
  static constexpr imsize_t half_w = w >> 1;
  static constexpr imsize_t half_h = h >> 1;
  using up_t = Pyramid<half_w, half_h>;
  MEMBER_INLINE void build_manual(); // Max-pooling (directional flags)
  MEMBER_INLINE void build_eigen(EigenMap<w, h> const& lower_map); // Mean-pooling (automatic)
private:
  uint8_t _array[h][w]; // NOT uint8_t**, thankfully--contiguous row-major format
  uint8_t _up_raw[pyrsize(half_w, half_h)]; // Please never access directly: use up()
  Pyramid(uint8_t *const __restrict src);
public:
  Pyramid() = delete;
  Pyramid(Pyramid const&) = delete;
  Pyramid(uint8_t src[h][w]) : Pyramid{&src[0][0]} {}
  Pyramid(vision::NaoImage<w, h> const& src) : Pyramid{src.internal.data()} {}
  MEMBER_INLINE uint8_t& operator()(imsize_t x, imsize_t y) { return _array[y][x]; }
  MEMBER_INLINE up_t& up() { return *reinterpret_cast<up_t*>(_up_raw); }
  // The coolest thing is that it doesn't even matter if we call up() one or two or n times too many--
  // it'll still return the same 0-element Pyramid!
  // We just need some kind of minimal (preferably compile-time) bounds checking in public methods
};
#pragma clang diagnostic pop

template <imsize_t w, imsize_t h>
Pyramid<w, h>::Pyramid(uint8_t *const __restrict src) {
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



template <imsize_t w, imsize_t h>
void Pyramid<w, h>::build_manual() {
  // Assumes we've already filled `_array` with valid image data

  static_assert(sizeof(Pyramid<w, h>) == pyrsize(w, h), PYRAMID_ERROR);
  
  // NOTE: AN INFINITE CONSTRUCTOR LOOP DOES *NOT* THROW A COMPILE-TIME ERROR
  // Need to verify that we have > 0 pixels to fill, else infinite loop of constructing nothing
  if ((!half_w) || (!half_h)) { return; }

  // Allocating for use in loops
  imsize_t twice;
  uint8_t a, b;

  // TODO: randomize shift when odd size

  // One dimension at a time: first half the width
  uint8_t tmp[h][half_w];
  for (imsize_t y = 0; y < h; ++y) {
    for (imsize_t x = 0; x < half_w; ++x) {
      twice = x << 1;
      a = _array[y][twice    ];
      b = _array[y][twice | 1];
      tmp[y][x] = ((a >> 2) > (b >> 2)) ? a & ~1 : b | 1; // Set a bit to represent left/right provenience (thx Sydney)
    }
  }

  // And half that, directly into the next layer
  Pyramid<half_w, half_h>& dst = up();
  for (imsize_t y = 0; y < half_h; ++y) {
    for (imsize_t x = 0; x < half_w; ++x) {
      twice = y << 1;
      a = tmp[twice    ][x];
      b = tmp[twice | 1][x];
      dst(x, y) = ((a >> 2) > (b >> 2)) ? a & ~2 : b | 2;
    }
  }

  dst.build_manual();
}



template <imsize_t w, imsize_t h>
void Pyramid<w, h>::build_eigen(EigenMap<w, h> const& lower_map) {
  static_assert(sizeof(Pyramid<w, h>) == pyrsize(w, h), PYRAMID_ERROR);
  if ((!half_w) || (!half_h)) { return; }
  uint8_t i0;
  EigenMap<half_w, half_h> upper_map{&up()._array[0][0]};
  if constexpr (w & 1) { i0 = rnd::bit(); } else { i0 = 0; }
  auto tmp = (lower_map(all, seqN(i0, half_w, 2)) >> 1) + (lower_map(all, seqN(i0 + 1, half_w, 2)) >> 1);
  if constexpr (h & 1) { i0 = rnd::bit(); } else { i0 = 0; }
  upper_map = (tmp(seqN(i0, half_h, 2), all) >> 1) + (tmp(seqN(i0 + 1, half_h, 2), all) >> 1);
}



} // namespace wasserstein

#endif // WASSERSTEIN_PYRAMID_HPP_

#else // WASSERSTEIN_ENABLED
#pragma message("Skipping pyramid.hpp; wasserstein module disabled")
#endif // WASSERSTEIN_ENABLED
