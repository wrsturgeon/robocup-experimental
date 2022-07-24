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
  static constexpr uint32_t npx = w * h;
  static constexpr imsize_t half_w = w >> 1;
  static constexpr imsize_t half_h = h >> 1;
  using up_t = Pyramid<half_w, half_h>;
private:
  uint8_t _array[h][w]; // NOT uint8_t**, thankfully--contiguous row-major format
  uint8_t _up_raw[pyrsize(half_w, half_h)]; // Please never access directly: use up()
public:
  Pyramid(Pyramid const&) = delete;
  Pyramid(Pyramid&&) = delete;
  Pyramid& operator=(Pyramid const&) = delete;
  Pyramid& operator=(Pyramid&&) = delete;
  Pyramid() = delete;
  Pyramid(uint8_t const *const __restrict src);
  up_t* up() { return reinterpret_cast<up_t*>(_up_raw); }
  // The coolest thing is that it doesn't even matter if we call up() one or two or n times too many--
  // it'll still return the same 0-element Pyramid!
  // We just need some kind of minimal (preferably compile-time) bounds checking in public methods
};

static_assert(!sizeof(Pyramid<0, 0>), "Pyramid class must have only 2 allocated members: _array & _up_raw");

template <imsize_t w, imsize_t h>
Pyramid<w, h>::Pyramid(uint8_t const *const __restrict src) {
  memcpy(_array, src, npx);
  // Build upward
}



} // namespace wasserstein

#endif // WASSERSTEIN_PYRAMID_HPP_

#else // WASSERSTEIN_ENABLED
#pragma message("Skipping pyramid.hpp; wasserstein module disabled")
#endif // WASSERSTEIN_ENABLED
