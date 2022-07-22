#if VISION_ENABLED
#ifndef VISION_WASSERSTEIN_HPP_
#define VISION_WASSERSTEIN_HPP_

#include <stdint.h>

#include <eigen.hpp>

#include <vision/image_api.hpp>

namespace vision {



// Forward declaration
template <pxidx_t w, pxidx_t h>
class Wasserstein;



/**
 * Level in an image-pyramid of height (w, h)
 * Subclasses Eigen::Matrix<uint8_t, w, h, RowMajor>
 * Only one channel (for our purposes, lumped difference)
 * Sole addition to the class is an `index` method
 *   that gives its index in bytes, assuming
 *   each non-base level is after the one immediately larger.
 */
template <pxidx_t w, pxidx_t h, uint8_t i>
class Level : public Eigen::Matrix<uint8_t, (w >> i), (h >> i), Eigen::StorageOptions::RowMajor> {
protected:
  friend class Wasserstein<w, h>;
  INLINE constexpr size_t index() {
    using last_t = Level<w, h, i - 1>; // recursively
    return i ? sizeof(last_t) + last_t::index() : 0;
  }
};



/**
 * Position-wise Wasserstein ("earth-mover") distance in an image.
 * Takes a pixel value and a location, then estimates how far you'd have to go to be right.
 */
template <pxidx_t w, pxidx_t h>
class Wasserstein {
public:
  Wasserstein() { for (uint8_t i = 0; i < n_levels; ++i) std::construct_at(level<i>()); }
  ~Wasserstein() { for (uint8_t i = 0; i < n_levels; ++i) std::destroy_at(level<i>()); }
  void init(NaoImage<w, h> const& im);
protected:
  static constexpr uint8_t n_levels = util::lgp1((h > w) ? h : w);
  template <uint8_t i> MEMBER_INLINE constexpr Level<w, h, i>* level() const;

  // Raw byte array--please never directly access
  // See https://en.cppreference.com/w/cpp/memory/construct_at
  uint8_t _storage[Level<w, h, n_levels>::index()];
};

/**
 *  This deserves some explanation.
 *  C++ arrays mandate that each element have the same type, even template arguments.
 *  But for this class we need a (variable, but compile-time)-sized array of levels,
 *    each of which has a different size (as a compile-time template argument).
 *  So we do what we would normally do in C:
 *    whip up an uninitialized byte array of known size,
 *    then initialize it after the fact (but ASAP).
 *  I initially thought `Eigen::Matrix` instances would all be the same size
 *    (i.e. initialize their data on the heap), but that's not the case;
 *    however, this still works perfectly, since the _offset_ in bytes
 *    is known a priori (add the last element's size to the last offset, recursively).
 */
template <pxidx_t w, pxidx_t h>
template <uint8_t i>
MEMBER_INLINE constexpr Level<w, h, i>* Wasserstein<w, h>::level() const {
  static_assert(i < n_levels, "Wasserstein::level(): i out of range");
  return reinterpret_cast<Level<w, h, i>*>(_storage + Level<w, h, i>::index());
}



} // namespace vision

#endif // VISION_WASSERSTEIN_HPP_

#else // VISION_ENABLED
#pragma message("Skipping wasserstein.hpp; vision module disabled")
#endif // VISION_ENABLED
