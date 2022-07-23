#if WASSERSTEIN_ENABLED
#ifndef WASSERSTEIN_PYRAMID_HPP_
#define WASSERSTEIN_PYRAMID_HPP_

#include <iostream>
#include <eigen.hpp>

namespace wasserstein {

using imsize_t = uint16_t;



static constexpr size_t pyrsize(imsize_t w, imsize_t h) { return (w && h) ? (w * h) + pyrsize(w >> 1, h >> 1) : 0; }

template <imsize_t w, imsize_t h>
class Pyramid {
public:
  Pyramid<(w >> 1), (h >> 1)> const& up() { return *reinterpret_cast<Pyramid<(w >> 1), (h >> 1)>*>(_internal_storage); }
  // The coolest thing is that it doesn't even matter if we call up() one or two or n times too many--
  // it'll still return the same 0-element Pyramid!
  // We just need some kind of minimal (preferably compile-time) bounds checking in public methods
private:
  uint8_t _array[w * h];
  uint8_t _internal_storage[pyrsize(w >> 1, h >> 1)]; // Please never access directly
protected:
  // Eigen::Map<Eigen::Matrix<uint8_t, w, h>> array{_array}; // Adds stack overhead
};

static_assert(!sizeof(Pyramid<0, 0>), "Pyramid class must have only 2 non-function members: _array & _internal_storage");



} // namespace wasserstein

#endif // WASSERSTEIN_PYRAMID_HPP_

#else // WASSERSTEIN_ENABLED
#pragma message("Skipping pyramid.hpp; wasserstein module disabled")
#endif // WASSERSTEIN_ENABLED
