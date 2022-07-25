#if WASSERSTEIN_ENABLED
#ifndef WASSERSTEIN_PYRAMID_HPP_
#define WASSERSTEIN_PYRAMID_HPP_

#include <iostream>
#include <string.h>

#include <eigen.hpp>

namespace wasserstein {

using imsize_t = uint16_t;



static constexpr size_t pyrsize(imsize_t w, imsize_t h) { return (w && h) ? (w * h) + pyrsize(w >> 1, h >> 1) : 0; }

template <imsize_t w, imsize_t h>
class Pyramid {
protected:
  static constexpr uint32_t n_px = w * h;
  static constexpr imsize_t half_w = w >> 1;
  static constexpr imsize_t half_h = h >> 1;
  using up_t = Pyramid<half_w, half_h>;

  // Allow layers beneath us to access `build()`
  friend class Pyramid<(w << 1)    , (h << 1)    >;
  friend class Pyramid<(w << 1)    , (h << 1) | 1>;
  friend class Pyramid<(w << 1) | 1, (h << 1)    >;
  friend class Pyramid<(w << 1) | 1, (h << 1) | 1>;
  MEMBER_INLINE void build();

private:
  // C++ standard guarantees identical order in memory (_array always first)
  uint8_t _array[h][w]; // NOT uint8_t**, thankfully--contiguous row-major format
  uint8_t _up_raw[pyrsize(half_w, half_h)]; // Please never access directly: use up()

public:
  Pyramid() = delete;
  Pyramid(Pyramid const&) = delete;
  Pyramid(uint8_t const src[h][w]);
  MEMBER_INLINE uint8_t& operator()(imsize_t x, imsize_t y) { return _array[y][x]; }
  MEMBER_INLINE up_t& up() { return *reinterpret_cast<up_t*>(_up_raw); }
  // The coolest thing is that it doesn't even matter if we call up() one or two or n times too many--
  // it'll still return the same 0-element Pyramid!
  // We just need some kind of minimal (preferably compile-time) bounds checking in public methods
};

#define PYRAMID_ERROR "Pyramid class must have only 2 allocated members: _array & _up_raw"
static_assert(!sizeof(Pyramid<0, 0>), PYRAMID_ERROR);
static_assert(sizeof(Pyramid<1, 1>) == 1, PYRAMID_ERROR);
static_assert(sizeof(Pyramid<IMAGE_W, IMAGE_H>) == pyrsize(IMAGE_W, IMAGE_H), PYRAMID_ERROR);
#undef PYRAMID_ERROR

template <imsize_t w, imsize_t h>
Pyramid<w, h>::Pyramid(uint8_t const src[h][w]) {
  memcpy(_array, src, n_px);
  build();
}

template <imsize_t w, imsize_t h>
void Pyramid<w, h>::build() {
  // Assumes we've already filled `_array` with valid image data
  
  // NOTE: AN INFINITE CONSTRUCTOR LOOP DOES *NOT* THROW A COMPILE-TIME ERROR
  // Need to verify that we have > 0 pixels to fill, else infinite loop of constructing nothing
  if ((!half_w) || (!half_h)) { return; }

  // Allocating for use in loops
  imsize_t twice;
  uint8_t a, b;

  // TODO: randomize shift when odd size
  // TODO: Look into using Eigen (rn I don't think it's possible)

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

  dst.build();
}



} // namespace wasserstein

#endif // WASSERSTEIN_PYRAMID_HPP_

#else // WASSERSTEIN_ENABLED
#pragma message("Skipping pyramid.hpp; wasserstein module disabled")
#endif // WASSERSTEIN_ENABLED
